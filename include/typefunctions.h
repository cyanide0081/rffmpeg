#ifndef H_TYPE_FUNCS
#define H_TYPE_FUNCS

#include "libs.h"

void destroyArguments(arguments *arguments);

formattedTime formatTime(double seconds);

void trimSpaces(char16_t *string);

#endif