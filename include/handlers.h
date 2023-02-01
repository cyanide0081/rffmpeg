#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"
#include "types.h"

int preventFilenameOverwrites(char16_t *pureFilename, const char16_t *outputFormat, const char16_t *path);

errorCode_t handleErrors(arguments_t *arguments);

#endif