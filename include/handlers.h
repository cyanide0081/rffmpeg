#ifndef H_HANDLERS
#define H_HANDLERS

#include "libs.h"

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat, const char *path);

errorCode_t handleErrors(char *arguments[]);

#endif