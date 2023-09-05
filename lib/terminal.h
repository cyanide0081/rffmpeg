#ifndef H_TERMINAL
#define H_TERMINAL

#include "libs.h"
#include "types.h"

#include <inttypes.h>

void printerr(const char *message, const char *descriptor);

void displayEndDialog(processInfo *processInformation);

#ifdef _WIN32

int restoreConsoleMode(DWORD originalConsoleMode);

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

#endif  /* _WIN32 */

#endif // H_TERMINAL
