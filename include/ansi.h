#ifndef ANSI_H
#define ANSI_H


#define ANSI_CURSOR_HIDE "\033[?25l"
#define ANSI_CURSOR_SHOW "\033[?25h"
#define ANSI_CLEAR "\x1b[2J"

#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_CLEAR "\x1b[2J"

// WARN: THIS IS NOT x;y, it's y;x
#define ANSI_POS(row, col) "\x1b[" row ";" col "H"

#define ANSI_CURSOR_HIDE "\033[?25l"

#endif // ANSI_H
