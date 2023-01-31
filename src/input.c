#include "../include/input.h"

int parseArgumentsFromTerminal(char16_t *arguments[], bool *options) {
    DWORD charactersRead = 0;
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);

    wprintf_s(u"%ls > %lsInput path: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments[ARG_INPATH], PATHBUF, &charactersRead, NULL);
    arguments[ARG_INPATH][wcscspn(arguments[ARG_INPATH], u"\r\n")] = u'\0'; // Remove trailing fgets() newline

    wprintf_s(u"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments[ARG_INFORMAT], BUFFER, &charactersRead, NULL);
    arguments[ARG_INFORMAT][wcscspn(arguments[ARG_INFORMAT], u"\r\n")] = u'\0';

    wprintf_s(u"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments[ARG_INPARAMETERS], BUFFER, &charactersRead, NULL);
    arguments[ARG_INPARAMETERS][wcscspn(arguments[ARG_INPARAMETERS], u"\r\n")] = u'\0';

    wprintf_s(u"%ls > %lsOutput extension: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments[ARG_OUTFORMAT], SHORTBUF, &charactersRead, NULL);
    arguments[ARG_OUTFORMAT][wcscspn(arguments[ARG_OUTFORMAT], u"\r\n")] = u'\0';

    char16_t optionsString[BUFFER];

    wprintf_s(u"%ls > %lsAdditional flags: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, optionsString, BUFFER, &charactersRead, NULL);
    optionsString[wcscspn(optionsString, u"\r\n")] = u'\0';

    uint16_t numberOfOptions = 0;
    char16_t *optionsTokenized[MAX_OPTS];
    char16_t *parserState;
    char16_t *token = wcstok_s(optionsString, u", ", &parserState);

    while (token) {
        optionsTokenized[numberOfOptions] = calloc(SHORTBUF, sizeof(char));
        wcscpy_s(optionsTokenized[numberOfOptions++], SHORTBUF, token);    

        token = wcstok_s(NULL, u" ", &parserState);
    }

    if (numberOfOptions > 0) {
        parseArguments(numberOfOptions, (const char16_t**)optionsTokenized, arguments, options, false, true);
    }

    wprintf_s(COLOR_DEFAULT);

    for (int i = 0; i < numberOfOptions; ++i)
        free(optionsTokenized[i]);

    return EXIT_SUCCESS;
}

void parseArguments(const int count, const char16_t *rawArguments[], char16_t *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions) {
    if (parseArguments) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        for (size_t i = 1; i < count; ++i) {
            if (wcscmp(rawArguments[i], u"-path") == 0) {
                wcsncpy_s(parsedArguments[ARG_INPATH], PATHBUF - 1, rawArguments[++i], PATHBUF);
            } else if (wcscmp(rawArguments[i], u"-fmt") == 0) {
                wcsncpy_s(parsedArguments[ARG_INFORMAT], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (wcscmp(rawArguments[i], u"-opts") == 0) {
                wcsncpy_s(parsedArguments[ARG_INPARAMETERS], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (wcscmp(rawArguments[i], u"-ext") == 0) {
                wcsncpy_s(parsedArguments[ARG_OUTFORMAT], SHORTBUF - 1, rawArguments[++i], SHORTBUF);
            }
        }
    }

    if (parseOptions) {
        /* fmt: --help, --newfolder=foldername, --delete, --norecursion, --overwrite, */
        for (size_t i = 0; i < count; ++i) {
            if (wcscmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
                parsedOptions[OPT_DISPLAYHELP] = true;
            } else if (wcsstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
                parsedOptions[OPT_MAKENEWFOLDER] = true;

                char16_t *argumentBuffer = wcsdup(rawArguments[i]); // duplicate argument wcsing for analysis

                char16_t *parserState;
                char16_t *delimiter = u"=";
                char16_t *token = wcstok_s(argumentBuffer, delimiter, &parserState);

                /* If there's an '=' sign, pass the string after it to the foldername argument */
                if ((token = wcstok_s(NULL, delimiter, &parserState)) != NULL) {
                    parsedOptions[OPT_CUSTOMFOLDERNAME] = true;
                    wcscpy_s(parsedArguments[ARG_NEWFOLDERNAME], PATHBUF, token);
                }

                free(argumentBuffer);
            } else if (wcscmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
                parsedOptions[OPT_DELETEOLDFILES] = true;
            } else if (wcscmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
                parsedOptions[OPT_DISABLERECURSION] = true;
            } else if (wcscmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
                parsedOptions[OPT_FORCEOVERWRITE] = true;
            }
        }
    }
}

int allocateArgumentBuffers(char16_t *arguments[]) {   
    arguments[ARG_INPATH]        = calloc(PATHBUF, sizeof(char16_t));
    arguments[ARG_INFORMAT]      = calloc(BUFFER, sizeof(char16_t));
    arguments[ARG_INPARAMETERS]  = calloc(BUFFER, sizeof(char16_t));
    arguments[ARG_OUTFORMAT]     = calloc(SHORTBUF, sizeof(char16_t));
    arguments[ARG_NEWFOLDERNAME] = calloc(PATHBUF, sizeof(char16_t));

    return EXIT_SUCCESS;
}

int freeArgumentBuffers(char16_t *arguments[]) {
    for (size_t i = 0; i < MAX_ARGS; ++i) {        
        free(arguments[i]);
        arguments[i] = NULL;    
    }

    return EXIT_SUCCESS;
}