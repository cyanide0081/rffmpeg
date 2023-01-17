#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include "libs.h"

void setup(void);
void prompt(const char *format);

size_t appendDotToString(char *string, char *control);

int handleErrors(char **arguments);
void printError(const char *msg);

char **parseArguments(int count, const char *arguments[], char *destination[]);
bool *parseOptions(int count, const char *arguments[], bool destination[]);

int preventFilenameOverwrites(char *pureFilename, char *outputFormat, char *path);

#endif