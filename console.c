#include "console.h"

int getInputFromConsole(char *arguments[], bool *options) {
    printf_s(u8"%s > %sInput path: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fscanf_s(stdin, "%s", arguments[ARG_INPATH], PATHBUF);
    arguments[ARG_INPATH][strcspn(arguments[ARG_INPATH], u8"\r\n")] = L'\0'; // Remove trailing fgets() newline
    
    printf_s(u8"%s > %sTarget format(s): %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fscanf_s(stdin, "%s", arguments[ARG_INFORMAT], BUFFER);
    arguments[ARG_INFORMAT][strcspn(arguments[ARG_INFORMAT], u8"\r\n")] = L'\0';

    printf_s(u8"%s > %sFFmpeg options: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fscanf_s(stdin, "%s", arguments[ARG_INPARAMETERS], BUFFER);
    arguments[ARG_INPARAMETERS][strcspn(arguments[ARG_INPARAMETERS], u8"\r\n")] = L'\0';

    printf_s(u8"%s > %sOutput format: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fscanf_s(stdin, "%s", arguments[ARG_OUTFORMAT], SHORTBUF);
    arguments[ARG_OUTFORMAT][strcspn(arguments[ARG_OUTFORMAT], u8"\r\n")] = L'\0';

    char optionsString[BUFFER];

    printf_s(u8"%s > %sAdditional modes: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fscanf_s(stdin, "%s", optionsString, BUFFER);
    optionsString[strcspn(optionsString, u8"\r\n")] = L'\0';

    printf_s(u8"\n");

    uint16_t numberOfOptions = 0;
     
    char *optionsTokenized[MAX_OPTS];
    char *parserState;
    char *token = strtok_s(optionsString, u8" ", &parserState);

    while (token) {
        optionsTokenized[numberOfOptions] = calloc(SHORTBUF, sizeof(char));

        strcpy_s(optionsTokenized[numberOfOptions++], SHORTBUF, token);

        token = strtok_s(NULL, u8" ", &parserState);
    }

    if (numberOfOptions > 0) {
        parseArguments(numberOfOptions, (const char**)optionsTokenized, arguments, options, false, true);
    }

    printf_s(COLOR_DEFAULT);

    for (int i = 0; i < numberOfOptions; ++i)
        free(optionsTokenized[i]);

    return EXIT_SUCCESS;
}