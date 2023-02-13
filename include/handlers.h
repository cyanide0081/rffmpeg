#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "terminal.h"

int preventFilenameOverwrites(char16_t *pureFilename, const char16_t *outputFormat, const char16_t *path);

int handleErrors(arguments *arguments);

#endif // H_HANDLERS