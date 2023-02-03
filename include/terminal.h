#ifndef H_TERMINALFUNCTIONS
#define H_TERMINALFUNCTIONS

#include "libs.h"
#include "types.h"

void printError(const char16_t *msg);

int restoreConsoleMode(DWORD originalConsoleMode);

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void displayEndDialog(processInfo *processInformation);

#endif