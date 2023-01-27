#include "console.h"

int getInputFromConsole(wchar_t *arguments[], bool *options) {
    wprintf_s(L"%ls > %lsInput path: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgetws(arguments[ARG_INPATH], PATHBUF, stdin);
    arguments[ARG_INPATH][wcscspn(arguments[ARG_INPATH], L"\r\n")] = L'\0'; // Remove trailing fgets() newline
    
    wprintf_s(L"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgetws(arguments[ARG_INFORMAT], BUFFER, stdin);
    arguments[ARG_INFORMAT][wcscspn(arguments[ARG_INFORMAT], L"\r\n")] = L'\0';

    wprintf_s(L"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgetws(arguments[ARG_INPARAMETERS], BUFFER, stdin);
    arguments[ARG_INPARAMETERS][wcscspn(arguments[ARG_INPARAMETERS], L"\r\n")] = L'\0';

    wprintf_s(L"%ls > %lsOutput format: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgetws(arguments[ARG_OUTFORMAT], SHORTBUF, stdin);
    arguments[ARG_OUTFORMAT][wcscspn(arguments[ARG_OUTFORMAT], L"\r\n")] = L'\0';

    wchar_t optionsString[BUFFER];

    wprintf_s(L"%ls > %lsAdditional modes: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgetws(optionsString, BUFFER, stdin);
    optionsString[wcscspn(optionsString, L"\r\n")] = L'\0';

    wprintf_s(L"\n");

    uint16_t numberOfOptions = 0;
     
    wchar_t *optionsTokenized[MAX_OPTS];
    wchar_t *parserState;
    wchar_t *token = wcstok_s(optionsString, L" ", &parserState);

    while (token) {
        optionsTokenized[numberOfOptions] = calloc(SHORTBUF, sizeof(wchar_t));

        wcscpy_s(optionsTokenized[numberOfOptions++], SHORTBUF, token);

        token = wcstok_s(NULL, L" ", &parserState);
    }

    if (numberOfOptions > 0) {
        parseArguments(numberOfOptions, (const wchar_t**)optionsTokenized, arguments, options, false, true);
    }

    wprintf_s(COLOR_DEFAULT);

    for (int i = 0; i < numberOfOptions; ++i)
        free(optionsTokenized[i]);

    return EXIT_SUCCESS;
}