#include "../include/types.h"
#include "../include/terminal.h"

arguments *initializeArguments(void) {
    arguments *instance = xcalloc(1, sizeof(*instance));

    instance->inPaths = xcalloc(LIST_BUFFER, sizeof(char*));
    instance->inFormats = xcalloc(LIST_BUFFER, sizeof(char*));
    
    return instance;
}

void destroyArguments(arguments *args) {
    if (args == NULL)
        return;

    for (int i = 0; args->inPaths[i] != NULL; i++) 
        free(args->inPaths[i]);

    for (int i = 0; args->inFormats[i] != NULL; i++)
        free(args->inFormats[i]);

    free(args->inPaths);
    free(args->inFormats);
    free(args->ffOptions);
    free(args->outFormat);
    free(args->customFolderName);

    free(args);
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
        printError("not enough memory", strerror(errno));

        exit(errno); // Immediate termination
    }

    // #ifdef DEBUG
    //     static size_t totalBytes = 0;

    //     totalBytes += numberOfElements * sizeOfElements;

    //     printf("\t> Total memory allocation (xcalloc): %zuKB\n\n", totalBytes / 0x400);
    // #endif

    return memory;
}

char *asprintf(const char *format, ...) {
    char *string = NULL;

    /* 2 va_lists for 2 calls to sprintf() */
    va_list args, args2;

    va_start(args, format);
    va_copy(args2, args);
    
    size_t bytes = vsnprintf(NULL, 0, format, args) + 1;

    string = xcalloc(bytes, sizeof(char));

    vsprintf(string, format, args2);

    va_end(args);
    va_end(args2);

    return string;
}