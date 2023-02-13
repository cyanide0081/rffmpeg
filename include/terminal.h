#ifndef H_TERMINAL
#define H_TERMINAL

#include "libs.h"
#include "types.h"

void printError(const char16_t *msg);

int restoreConsoleMode(DWORD originalConsoleMode);

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void displayEndDialog(processInfo *processInformation);

#endif // H_TERMINAL