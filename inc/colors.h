// Defines common SGR (Select Graphic Rendition) parameters
// For more info, please go to http://en.wikipedia.org/wiki/ANSI_escape_code

#ifndef JOS_INC_COLORS_H
#define JOS_INC_COLORS_H

#define BLK_FG "\x1B[30m"
#define RED_FG "\x1B[31m"
#define GRN_FG "\x1B[32m"
#define YEL_FG "\x1B[33m"
#define BLU_FG "\x1B[34m"
#define MAG_FG "\x1B[35m"
#define CYN_FG "\x1B[36m"
#define WHT_FG "\x1B[37m"

#define BLK_BG "\x1B[40m"
#define RED_BG "\x1B[41m"
#define GRN_BG "\x1B[42m"
#define YEL_BG "\x1B[43m"
#define BLU_BG "\x1B[44m"
#define MAG_BG "\x1B[45m"
#define CYN_BG "\x1B[46m"
#define WHT_BG "\x1B[47m"

#define BOLD	"\x1B[1m"
#define U_LINE	"\x1B[4m"
#define BLINK	"\x1B[5m"
#define INVERSE	"\x1B[7m"
#define CONCEAL "\x1B[8m"

#define RST_COLOR "\x1b[39;49m"
#define RST_ALL	"\x1b[0m"
#define RST	RST_ALL

#endif

