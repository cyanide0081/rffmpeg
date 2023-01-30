#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"

int parseArgumentsFromTerminal(char *arguments[], bool *options);

void parseArguments(const int count, const char *rawArguments[], char *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions);

#endif