#ifndef H_INPUTFUNCTIONS
#define H_INPUTFUNCTIONS

#include "libs.h"

int parseArgumentsFromTerminal(char16_t *arguments[], bool *options);

void parseArguments(const int count, const char16_t *rawArguments[], char16_t *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions);

int allocateArgumentBuffers(char16_t *arguments[]);

int freeArgumentBuffers(char16_t *arguments[]);

#endif