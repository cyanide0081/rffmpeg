#include "../include/types.h"
#include "../include/terminal.h"

arguments *initializeArguments(void) {
    arguments *instance = xcalloc(1, sizeof(*instance));

    instance->inPaths = xcalloc(SHORTBUF, sizeof(char*));
    instance->inFormats = xcalloc(SHORTBUF, sizeof(char*));
    
    instance->ffOptions = xcalloc(BUFFER, sizeof(char));
    instance->outFormat = xcalloc(SHORTBUF, sizeof(char));
    instance->customFolderName = xcalloc(FILE_BUFFER, sizeof(char));
    
    return instance;
}

void destroyArguments(arguments *arguments) {
    if (arguments == NULL)
        return;

    for (int i = 0; arguments->inPaths[i]; i++) 
        free(arguments->inPaths[i]);

    for (int i = 0; arguments->inFormats[i]; i++)
        free(arguments->inFormats[i]);

    free(arguments->inPaths);
    free(arguments->inFormats);
    free(arguments->ffOptions);
    free(arguments->outFormat);
    free(arguments->customFolderName);

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
void trimWhiteSpaces(char *string) {
    size_t length = strlen(string);

    char *start = string;

    while (isspace(*start))
        start++;

    char *end = string + length - 1;

    /* Replace spaces with 0s */
    while (isspace(*end))  {
        *end-- = u'\0';  
    }

    /* Shift spaceless part to the start */ 
    if (start != string) {
        memmove(string, start, length + 1);  
        memset(string + strlen(string) + 1, u'\0', start - string); // Fill the extra bytes with 0s
    }
}

/* Extended calloc() that terminates program on failure */
void *xcalloc(size_t numberOfElements, size_t sizeOfElements) {
    void *memory = calloc(numberOfElements, sizeOfElements);

    if (memory == NULL) {
        printError("exception while allocating memory");

        exit(errno); // Immediate termination
    }

    return memory;
}