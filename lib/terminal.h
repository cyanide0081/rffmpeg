#ifndef H_TERMINAL
#define H_TERMINAL

#include <libs.h>
#include <types.h>

#include <inttypes.h>

#define printErr(msg, dsc)                                                     \
    fprintf(\
        stderr,\
        "%s ERROR: %s%s (%s%s%s)\n\n",\
        CHARCOLOR_RED,\
        CHARCOLOR_WHITE, msg,\
        CHARCOLOR_RED, dsc,\
        COLOR_DEFAULT\
    );\

void displayEndDialog(processInfo *processInformation);

#ifdef _WIN32

int restoreConsoleMode(DWORD originalConsoleMode);

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

#endif  /* _WIN32 */

#endif // H_TERMINAL
