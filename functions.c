#include "functions.h"

size_t appendDotToString(char *string, size_t stringSize) {
    char buffer[SHORTBUF];
    size_t size = strlen(string) + strlen(".");

    strcpy_s(buffer, SHORTBUF - 1, ".");
    strcat_s(buffer, SHORTBUF - 1, string);
    strcpy_s(string, stringSize - 1, buffer);

    return size;
}

void printError(const char *msg) {
    fprintf(stderr, "%sERROR: %s%s%s\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

char **parseArguments(int count, const char *arguments[], char *destination[]) {
    /* fmt: -i <path> -f <container> -p <params> -o <container> */
    const char **a = arguments;
    char **list = destination;

    for (int i = 1; i < count; ++i) {
        if (strcmp(a[i], "-i") == 0) {
            list[ARG_PATH] = malloc(MAX_PATH);
            strncpy_s(list[ARG_PATH], MAX_PATH - 1, a[++i], MAX_PATH);
        }

        if (strcmp(a[i], "-f") == 0) {
            list[ARG_FORMAT] = malloc(BUFFER);
            strncpy_s(list[ARG_FORMAT], BUFFER - 1, a[++i], BUFFER);
        }

        if (strcmp(a[i], "-p") == 0) {
            list[ARG_PARAMS] = malloc(BUFFER);
            strncpy_s(list[ARG_PARAMS], BUFFER - 1, a[++i], BUFFER);
        }

        if (strcmp(a[i], "-o") == 0) {
            list[ARG_OUTPUT] = malloc(SHORTBUF);
            strncpy_s(list[ARG_OUTPUT], SHORTBUF - 1, a[++i], SHORTBUF);
        }
    }

    return list;
}

bool *parseOptions(int count, const char *arguments[], bool destination[]) {
    /* fmt: --help (duh) --n (folder) --d (delete old) --r (recursive) --y (overwrite) */
    const char **a = arguments;
    bool *list = destination;

    for (int i = 1; i < count; ++i) {

        if (strstr(a[i], "-h"))
            list[OPT_HELP] = true;

        if (strcmp(a[i], "/n") == 0)
            list[OPT_NEWFOLDER] = true;

        if (strcmp(a[i], "/d") == 0)
            list[OPT_DELETE] = true;

        if (strcmp(a[i], "/r") == 0)
            list[OPT_RECURSIVE] = true;

        if (strcmp(a[i], "/y") == 0)
            list[OPT_OVERWRITE] = true;
    }

    return list;
}

int preventFilenameOverwrites(char *pureFilename, char *outputFormat, char *path) {
    char *f = pureFilename;
    const char *o = outputFormat, *p = path;
    bool exists = false;
    char buf[BUFFER] = { '\0' };

    sprintf_s(buf, BUFFER, "%s\\%s%s", p, f, o);

    if (PathFileExistsA(buf)) {
        uint16_t index = 0;

        while (PathFileExistsA(buf)) {
            sprintf_s(buf, BUFFER, "%s\\%s-%03d%s", p, f, ++index, o);
        }

        sprintf_s(buf, BUFFER, "%s-%03d", f, index);
        strcpy_s(f, FILENAME_MAX, buf);
    }

    return EXIT_SUCCESS;
}

int handleErrors(char **arguments) {  
    if (!arguments[ARG_PATH] || *arguments[ARG_PATH] == 0) {
        arguments[ARG_PATH] = IDENTIFIER_NO_PATH;
    }

    if (!arguments[ARG_FORMAT] || *arguments[ARG_FORMAT] == 0) {
        printError("no input format (null)");
        return EXIT_FAILURE;
    }

    if (!arguments[ARG_OUTPUT] || *arguments[ARG_OUTPUT] == 0) {
        printError("no output format (null)");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}