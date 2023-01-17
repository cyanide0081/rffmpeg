#include "console.h"

int runInConsoleMode(char **arguments, bool *options) {
    system("@ECHO OFF");
    system("TITLE " PROGRAM_NAME " v" PROGRAM_VERSION);

    /* Start parsing */
    char **a = arguments;
    char optionsString[1][BUFFER];
    bool *o = options;

    printf("%s%s\n", CHARCOLOR_RED, fullTitle);
    putchar('\n');
    printf("%s > %sInput path: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    a[ARG_PATH] = malloc(MAX_PATH);
    fgets(a[ARG_PATH], PATH_MAX, stdin);
    a[ARG_PATH][strcspn(a[ARG_PATH], "\r\n")] = '\0'; // Remove trailing fgets() newline

    printf("%s > %sTarget format(s): %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    a[ARG_FORMAT] = malloc(BUFFER);
    fgets(a[ARG_FORMAT], BUFFER, stdin);
    a[ARG_FORMAT][strcspn(a[ARG_FORMAT], "\r\n")] = '\0';

    printf("%s > %sFFmpeg options: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    a[ARG_PARAMS] = malloc(BUFFER);
    fgets(a[ARG_PARAMS], BUFFER, stdin);
    a[ARG_PARAMS][strcspn(a[ARG_PARAMS], "\r\n")] = '\0';

    printf("%s > %sOutput format: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    a[ARG_OUTPUT] = malloc(SHORTBUF);
    fgets(a[ARG_OUTPUT], SHORTBUF, stdin);
    a[ARG_OUTPUT][strcspn(a[ARG_OUTPUT], "\r\n")] = '\0';

    printf("%s > %sAdditional modes: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgets(optionsString[0], BUFFER, stdin);
    optionsString[0][strcspn(optionsString[0], "\r\n")] = '\0'; // Same thing as above

    parseOptions(1, (const char**)optionsString, o); // macarrão

    puts(COLOR_DEFAULT);

    return EXIT_SUCCESS;
}