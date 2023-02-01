#ifndef H_TERMINALFUNCTIONS
#define H_TERMINALFUNCTIONS

#include "libs.h"
#include "types.h"

void printError(const char16_t *msg);

errno_t restoreConsoleMode(DWORD originalConsoleMode);

errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void displayEndDialog(processInfo_t *processInformation);

#endif