#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "terminal.h"

/* Currently useless error codes */
#define ERROR_NO_INPUT_FORMAT          81000
#define ERROR_NO_OUTPUT_FORMAT         81001
#define ERROR_FAILED_SETCONSOLEMODE    81002
#define ERROR_FAILED_TO_OPEN_DIRECTORY 81003

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat,
 const char *path);

int handleArgumentErrors(arguments *arguments);

int createTestProcess(void);

#endif // H_HANDLERS