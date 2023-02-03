#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"

errno_t parseArgumentsFromTerminal(arguments_t *arguments);

errno_t parseCommandLineArguments(const int count, const char16_t *rawArguments[], arguments_t *parsedArguments);

#endif