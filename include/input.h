#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"

int parseArgumentsFromTerminal(arguments *arguments);

int parseCommandLineArguments(const int count, const char16_t *rawArguments[], arguments *parsedArguments);

#endif