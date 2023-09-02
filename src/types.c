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

/* Returns a time structure formatted in hours, minutes and seconds */
fmtTime formatTime(double seconds) {
    fmtTime time = {
        .hours = seconds / 3600,
        .minutes = (seconds - (time.hours * 3600)) / 60,
        .seconds =
            (double)(seconds - (time.hours * 3600) - (time.minutes * 60)),
    };

    return time;
}

/* Trim leading and trailing empty characters from a string */
void trimSpaces(char *string) {
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
        memmove(string, start, strlen(start) + 1);
        memset(string + strlen(string) + 1, u'\0', start - string);
    }
}

/* Extended calloc() that terminates program on failure */
void *xcalloc(size_t numberOfElements, size_t sizeOfElements) {
    void *memory = calloc(numberOfElements, sizeOfElements);

    if (memory == NULL) {
        printError("not enough memory", strerror(errno));

        exit(errno); // Immediate termination
    }

    return memory;
}

char *asprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    size_t bytes = vsnprintf(NULL, 0, format, args) + 1;

    char *string = xcalloc(bytes, sizeof(char));

    va_end(args);
    va_start(args, format);

    vsprintf(string, format, args);

    va_end(args);

    return string;
}
