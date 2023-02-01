#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"
#include "types.h"

char16_t **parseArgumentsFromTerminal(size_t *outputArgumentsCount, char16_t *outputArguments[]);

arguments_t *parseCommandLineArguments(const int count, const char16_t *rawArguments[]);

#endif