// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).
	if (!((err & FEC_WR) && (uvpd[PDX(addr)] & PTE_P) &&
		  (uvpt[PGNUM(addr)] & PTE_COW))) {
		panic("Page fault access is not a write or to a COW page.");
	}

	// LAB 4: Your code here.

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.

	// LAB 4: Your code here.
	addr = (void *) PGROUNDDOWN((uintptr_t) addr);
	if ((r = sys_page_alloc(0, PFTEMP, PTE_U|PTE_P|PTE_W)) < 0) {
		panic("sys_page_alloc: failed to allocate a page.");
	}

	memmove(PFTEMP, addr, PGSIZE);

	if ((r = sys_page_map(0, PFTEMP, 0, addr, PTE_U|PTE_P|PTE_W)) < 0) {
		panic("sys_page_map: failed to map page.");
	}
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	pte_t pte = uvpt[pn];
	void *addr = (void *) (pn * PGSIZE);

	if ((pte & PTE_W) || (pte & PTE_COW)) {
		if ((r = sys_page_map(0, addr, envid, addr,
							  PTE_U|PTE_P|PTE_COW)) < 0) {
			panic("sys_page_map: failed to map page");
		}
		if ((r = sys_page_map(0, addr, 0, addr, PTE_U|PTE_P|PTE_COW)) < 0) {
			panic("sys_page_map: failed to map page");
		}
	} else {
		if ((r = sys_page_map(0, addr, envid, addr, PTE_U|PTE_P)) < 0) {
			panic("sys_page_map: failed to map page");
		}
	}
	
	return r;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	envid_t envid;

	// Set up our own COW page fault handler
	set_pgfault_handler(pgfault);

	// Create a child
	envid = sys_exofork();
	if (envid < 0) {
		panic("sys_exofork: %e", envid);
	}

	// if it's a child
    if (envid == 0) {
        // We're the child.
        // The copied value of the global variable 'thisenv'
        // is no longer valid (it refers to the parent!).
        // Fix it and return 0.
        thisenv = &envs[ENVX(sys_getenvid())];
        return 0;
    }

	// if it's a parent
	uintptr_t addr;

	for (addr = UTEXT; addr < UTOP - PGSIZE; addr += PGSIZE) {
		if ((uvpd[PDX(addr)] & PTE_P) &&
			(uvpt[PGNUM(addr)] & (PTE_P|PTE_U))) {
			if (duppage(envid, PGNUM(addr)) < 0) {
				panic("dupage: page remap failed");
			}
		}
	}

	if (sys_page_alloc(envid, (void *) (UXSTACKTOP - PGSIZE),
					   PTE_P|PTE_U|PTE_W) < 0) {
		panic("sys_page_alloc: failed to allocate page for "
			  "user exception stack.");
	}

	// Assembly language pgfault entrypoint defined in lib/pfentry.S.
	extern void _pgfault_upcall(void);

	if (sys_env_set_pgfault_upcall(envid, _pgfault_upcall) < 0) {
		panic("sys_env_set_pgfault_upcall: failed to set pgfault upcall "
			  "in child.");
	}

	if (sys_env_set_status(envid, ENV_RUNNABLE) < 0) {
		panic("sys_env_set_status: failed to set status for envid.");
	}

	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
