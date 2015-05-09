/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps


/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
.text

/* traps where CPU pushes error code */
TRAPHANDLER(SYSTEM_ERROR, T_DBLFLT); #8
TRAPHANDLER(INVALID_TSS, T_TSS); #10
TRAPHANDLER(SEGMENT_NOT_PRESENT, T_SEGNP); #11
TRAPHANDLER(STACK_EXCEPTION, T_STACK); #12
TRAPHANDLER(GENERAL_PROTECTION_FAULT, T_GPFLT); #13
TRAPHANDLER(PAGE_FAULT, T_PGFLT); #14

/* traps where CPU does not push error code */
TRAPHANDLER_NOEC(DIVIDE_ERROR, T_DIVIDE); #0
TRAPHANDLER_NOEC(DEBUG_EXCEPTIONS, T_DEBUG); #1
TRAPHANDLER_NOEC(NMI, T_NMI); #2
TRAPHANDLER_NOEC(BREAKPOINT, T_BRKPT); #3
TRAPHANDLER_NOEC(OVERFLOW, T_OFLOW); #4
TRAPHANDLER_NOEC(BOUNDS_CHECK, T_BOUND); #5
TRAPHANDLER_NOEC(INVALID_OPCODE, T_ILLOP); #6
TRAPHANDLER_NOEC(COPROCESSOR_NOT_AVAILABLE, T_DEVICE); #7
TRAPHANDLER_NOEC(COPROCESSOR_SEGMENT_OVERRUN, T_COPROC); #9 reserved
TRAPHANDLER_NOEC(RESERVED_15, T_RES); #15 reserved
TRAPHANDLER_NOEC(COPROCESSOR_ERROR, T_FPERR); #16

/* JOS (17-31 reserved) */
TRAPHANDLER_NOEC(ALIGNMENT_CHECK, T_ALIGN); #17
TRAPHANDLER_NOEC(MACHINE_CHECK, T_MCHK); #18
TRAPHANDLER_NOEC(SIMD_FP_ERROR, T_SIMDERR); #19

/* External Interrupts (32-47) */
TRAPHANDLER_NOEC(IRQ0_TIMER, IRQ_OFFSET + IRQ_TIMER);		    #32
TRAPHANDLER_NOEC(IRQ1_KBD, IRQ_OFFSET + IRQ_KBD);				#33
TRAPHANDLER_NOEC(IRQ4_SERIAL, IRQ_OFFSET + IRQ_SERIAL);			#36
TRAPHANDLER_NOEC(IRQ7_SPURIOUS, IRQ_OFFSET + IRQ_SPURIOUS);		#39
TRAPHANDLER_NOEC(IRQ14_IDE, IRQ_OFFSET + IRQ_IDE);				#46

/* Syscall */
TRAPHANDLER_NOEC(SYSCALL_TRAP, T_SYSCALL);						#48


/*
 * Lab 3: Your code here for _alltraps
 */
.globl _alltraps
 _alltraps:
	# Build trap frame
	pushl %ds
	pushl %es
	pushal

	# Setup data segments
	xor %ax, %ax # reset %ax
	movw $GD_KD, %ax
	mov %ax, %ds
	mov %ax, %es

	# Call trap(tp)
	pushl %esp
	call trap
	addl $0x4, %esp

.globl trapret
trapret:
	popal
	popl %es
	popl %ds
	addl $0x8, %esp	#trapno and error code
	iret # return from kernel space to user space

/*
 * Define the global symbols for each type of interrupt/exception
 */
.data
.globl int_vectors
int_vectors:
	# No Error Code
	.long DIVIDE_ERROR					#0
	.long DEBUG_EXCEPTIONS				#1
	.long NMI							#2
	.long BREAKPOINT					#3
	.long OVERFLOW						#4
	.long BOUNDS_CHECK					#5
	.long INVALID_OPCODE				#6
	.long COPROCESSOR_NOT_AVAILABLE		#7
	# Error Code - 0
	.long SYSTEM_ERROR					#8
	# No Error Code
	.long COPROCESSOR_SEGMENT_OVERRUN	#9
	# Error Code
	.long INVALID_TSS					#10
	.long SEGMENT_NOT_PRESENT			#11
	.long STACK_EXCEPTION				#12
	.long GENERAL_PROTECTION_FAULT		#13
	.long PAGE_FAULT					#14
	# No Error Code
	.long RESERVED_15					#15
	.long COPROCESSOR_ERROR				#16
	# 17 - 31 (reserved)
	.long ALIGNMENT_CHECK				#17
	.long MACHINE_CHECK					#18
	.long SIMD_FP_ERROR					#19
	.long 20
	.long 21
	.long 22
	.long 23
	.long 24
	.long 25
	.long 26
	.long 27
	.long 28
	.long 29
	.long 30
	.long 31
	# 32 - 48
	.long IRQ0_TIMER					#32
	.long IRQ1_KBD						#33
	.long 34
	.long 35
	.long IRQ4_SERIAL					#36
	.long 37
	.long 38
	.long IRQ7_SPURIOUS					#39
	.long 40
	.long 41
	.long 42
	.long 43
	.long 44
	.long 45
	.long IRQ14_IDE						#46
	.long 47
	# T_SYSCALL
	.long SYSCALL_TRAP	#48

