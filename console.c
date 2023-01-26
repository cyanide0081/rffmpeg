#include "console.h"

int runInConsoleMode(wchar_t *arguments[], bool *options) {
    _wsystem(L"@ECHO OFF");
    _wsystem(L"TITLE " PROGRAM_NAME L" " PROGRAM_VERSION);

    wprintf_s(L"%ls > %lsInput path: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    arguments[ARG_INPATH] = malloc(PATHBUF);
    fgetws(arguments[ARG_INPATH], PATHBUF, stdin);
    arguments[ARG_INPATH][wcscspn(arguments[ARG_INPATH], L"\r\n")] = L'\0'; // Remove trailing fgets() newline
    
    if (*arguments[ARG_INPATH] == L'\0') {
        free(arguments[ARG_INPATH]);
        arguments[ARG_INPATH] = NULL;
    }

    wprintf_s(L"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    arguments[ARG_INFORMAT] = malloc(BUFFER);
    fgetws(arguments[ARG_INFORMAT], BUFFER, stdin);
    arguments[ARG_INFORMAT][wcscspn(arguments[ARG_INFORMAT], L"\r\n")] = L'\0';

    wprintf_s(L"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    arguments[ARG_INPARAMETERS] = malloc(BUFFER);
    fgetws(arguments[ARG_INPARAMETERS], BUFFER, stdin);
    arguments[ARG_INPARAMETERS][wcscspn(arguments[ARG_INPARAMETERS], L"\r\n")] = L'\0';

    if (*arguments[ARG_INPARAMETERS] == L'\0') {
        free(arguments[ARG_INPARAMETERS]);
        arguments[ARG_INPARAMETERS] = NULL;
    }

    wprintf_s(L"%ls > %lsOutput format: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    arguments[ARG_OUTFORMAT] = malloc(SHORTBUF);
    fgetws(arguments[ARG_OUTFORMAT], SHORTBUF, stdin);
    arguments[ARG_OUTFORMAT][wcscspn(arguments[ARG_OUTFORMAT], L"\r\n")] = L'\0';

    wchar_t optionsString[BUFFER];

    wprintf_s(L"%ls > %lsAdditional modes: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    fgetws(optionsString, BUFFER, stdin);
    optionsString[wcscspn(optionsString, L"\r\n")] = L'\0';

    wprintf_s(L"\n");

    uint16_t numberOfOptions = 0;
     
    wchar_t *optionsTokenized[MAX_OPTS] = { NULL };
    wchar_t *parserState;
    wchar_t *token = wcstok_s(optionsString, L" ", &parserState);

    while (token) {
        optionsTokenized[numberOfOptions] = malloc(SHORTBUF);

        wcscpy_s(optionsTokenized[numberOfOptions++], SHORTBUF - 1, token);

        token = wcstok_s(NULL, L" ", &parserState);
    }

    parseOptions(numberOfOptions, (const wchar_t**)optionsTokenized, options);

    wprintf(COLOR_DEFAULT);

    for (int i = 0; optionsTokenized[i]; ++i)
        free(optionsTokenized[i]);

    return EXIT_SUCCESS;
}