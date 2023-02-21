#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "terminal.h"
#include "parsers.h"

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat, const char *path);

int handleArgumentErrors(arguments *arguments);

int createTestProcess(void);

int getCurrentOS(void);

#endif // H_HANDLERS