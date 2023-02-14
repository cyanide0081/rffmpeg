#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "terminal.h"

#define ERROR_NO_INPUT_FORMAT          81000
#define ERROR_NO_OUTPUT_FORMAT         81001
#define ERROR_FAILED_SETCONSOLEMODE    81002
#define ERROR_FAILED_TO_OPEN_DIRECTORY 81003

int preventFilenameOverwrites(char16_t *pureFilename, const char16_t *outFormat, const char16_t *path);

int handleArgumentErrors(arguments *arguments);

int createTestProcess(void);

#endif // H_HANDLERS