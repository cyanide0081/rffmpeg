#include "../include/types.h"
#include "../include/terminal.h"

arguments *initializeArguments(void) {
    arguments *instance = calloc(1, sizeof(*instance));

    if (instance == NULL) {
        printError(u"not enough memory");

        return NULL;
    }
    
    return instance;
}

void destroyArguments(arguments *arguments) {
    if (arguments == NULL)
        return;

    for (int i = 0; i < arguments->inputPathsCount; i++) 
        if (arguments->inputPaths[i])
            free(arguments->inputPaths[i]);

    for (int i = 0; i < arguments->inputFormatsCount; i++)
        if (arguments->inputFormats[i])
            free(arguments->inputFormats[i]);

    free(arguments);
}

formattedTime formatTime(double seconds) {
    formattedTime time;

    time.hours = seconds / 3600;
    time.minutes = (seconds - (time.hours * 3600)) / 60;
    time.seconds = (double)(seconds - (time.hours * 3600) - (time.minutes * 60));

    return time;
}

/* Trim leading and trailing empty characters from a string */
void trimWhiteSpaces(char16_t *string) {
    size_t length = wcslen(string);

    char16_t *start = string;

    while (isspace(*start))
        start++;

    char16_t *end = string + length - 1;

    /* Replace spaces with 0s */
    while (isspace(*end))  {
        *end-- = u'\0';  
    }

    /* Shift spaceless part to the start */ 
    if (start != string) {
        memmove(string, start, length + 1);  
        memset(string + wcslen(string) + 1, u'\0', start - string); // Fill the extra bytes with 0s
    }
}