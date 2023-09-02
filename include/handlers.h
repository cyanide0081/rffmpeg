#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "terminal.h"
#include "parsers.h"

int handleFileNameConflicts(char *pureFilename,
                            const char *outputFormat, const char *path);

int handleArgErrors(arguments *arguments);

int createTestProcess(void);

#endif // H_HANDLERS
