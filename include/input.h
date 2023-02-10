#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"

int parseConsoleInput(arguments *arguments);

int parseArguments(const int count, char16_t *rawArguments[], arguments *parsedArguments);

#endif