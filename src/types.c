#include "../lib/types.h"
#include "../lib/terminal.h"

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
        .hours = (int64_t)(seconds / 3600),
        .minutes = (int64_t)((seconds - (time.hours * 3600)) / 60),
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
        char errormsg[NAME_MAX] = "";
        strerror_s(errormsg, NAME_MAX, errno);
        printErr("not enough memory", errormsg);

        exit(errno);
    }

    return memory;
}

char *_asprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    size_t bytes = vsnprintf(NULL, 0, format, args) + 1;

    char *string = xcalloc(bytes, sizeof(char));

    va_end(args);
    va_start(args, format);

    vsprintf(string, format, args); // Ignore compiler deprecation warning here

    va_end(args);

    return string;
}

ssize_t getline(char **string, size_t *buffer, FILE *stream) {
    #define LARGE_BUF 4096

#ifdef _WIN32
    wchar_t wideBuf[LARGE_BUF];

    if (fgetws(wideBuf, LARGE_BUF, stream) == NULL)
        return -1;

    int size = UTF16toUTF8(wideBuf, -1, NULL, 0);
    char *buf = xcalloc(size, sizeof(char));

    UTF16toUTF8(wideBuf, -1, buf, size);
#else
    char buf[LARGE_BUF];

    if (fgets(buf, LARGE_BUF, stream) == NULL)
        return -1;
#endif

    trimSpaces(buf);

    if (*buffer == 0)
        *string = strdup(buf);
    else
        memccpy(*string, buf, '\0', *buffer);

    return strlen(*string);
}
