#ifndef H_PARSERS
#define H_PARSERS

#include "libs.h"
#include "types.h"
#include "terminal.h"

int parseConsoleInput(arguments *arguments);

int parseArguments(const int count, char16_t *rawArguments[], arguments *parsedArguments);

#endif // H_PARSERS