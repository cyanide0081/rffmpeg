#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include "libs.h"

void printError(const char *msg);
errorCode_t handleErrors(char *arguments[]);

errno_t clearConsoleWindow(void);
errno_t resetConsoleMode(DWORD originalConsoleMode);
errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode);

void parseArguments(const int count, const char *rawArguments[], char *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions);

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat, const char *path);
errorCode_t searchDirectory(const char *directory, char *arguments[], const bool *options, processInfo_t *runtimeData);

void displayEndDialog(processInfo_t *processInformation);

#endif