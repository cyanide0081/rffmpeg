#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include "libs.h"

void printError(const wchar_t *msg);
errorCode_t handleErrors(wchar_t *arguments[]);

errno_t clearConsoleWindow(void);
errno_t resetConsoleMode(DWORD originalConsoleMode);
errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void parseArguments(const int count, const wchar_t *rawArguments[], wchar_t *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions);

int preventFilenameOverwrites(wchar_t *pureFilename, const wchar_t *outputFormat, const wchar_t *path);
errorCode_t searchDirectory(const wchar_t *directory, wchar_t *arguments[], const bool *options, processInfo_t *runtimeData);

void displayEndDialog(processInfo_t *processInformation);

#endif