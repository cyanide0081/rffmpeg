#ifndef H_TERMINALFUNCTIONS
#define H_TERMINALFUNCTIONS

#include "libs.h"

void printError(const char *msg);

errno_t clearConsoleWindow(void);

errno_t resetConsoleMode(DWORD originalConsoleMode);

errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void displayEndDialog(processInfo_t *processInformation);

#endif