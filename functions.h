#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include "libs.h"

int handleErrors(wchar_t *arguments[]);
void printError(const wchar_t *msg);
errno_t clearConsoleWindow(void);
errno_t resetConsoleMode(DWORD originalConsoleMode);
errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

wchar_t **parseArguments(int count, const wchar_t *arguments[], wchar_t *destination[]);
bool *parseOptions(int count, const wchar_t *options[], bool destination[]);

int preventFilenameOverwrites(wchar_t *pureFilename, const wchar_t *outputFormat, const wchar_t *path);

int searchDirectory(const wchar_t *directory, wchar_t *arguments[], const bool *options, processInfo_t *runtimeData);

void displayEndDialog(processInfo_t *processInformation);

#endif