#ifndef H_TERMINAL
#define H_TERMINAL

#include "libs.h"
#include "types.h"

void printError(const char *msg);

void displayEndDialog(processInfo *processInformation);

#ifdef _WIN32
    int restoreConsoleMode(DWORD originalConsoleMode);

    int enableVirtualTerminalProcessing(PDWORD originalConsoleMode);
#endif

#endif // H_TERMINAL