#include "../include/input.h"

/* Gets the argument strings from a console menu and formats them to be parsed by parseCommandLineArguments() */
char16_t **parseArgumentsFromTerminal(size_t *outputArgumentsCount, char16_t *outputArguments[]) {
    size_t currentIndex = 0;
    DWORD charactersRead = 0;
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);

    wprintf_s(u"%ls > %lsInput path: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    outputArguments[currentIndex] = calloc(sizeof(u"-path"), sizeof(char16_t));
    wcscpy_s(outputArguments[currentIndex], PATHBUF, u"-path");
    ++currentIndex;

    outputArguments[currentIndex] = calloc(PATHBUF, sizeof(char16_t));

    ReadConsoleW(consoleInput, outputArguments[currentIndex], PATHBUF, &charactersRead, NULL);

    outputArguments[currentIndex][wcscspn(outputArguments[currentIndex], u"\r\n")] = u'\0'; // Remove trailing fgets() newline

    ++currentIndex;

    wprintf_s(u"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    outputArguments[currentIndex] = calloc(sizeof(u"-fmt"), sizeof(char16_t));
    wcscpy_s(outputArguments[currentIndex], PATHBUF, u"-fmt");
    ++currentIndex;

    outputArguments[currentIndex] = calloc(BUFFER, sizeof(char16_t));

    ReadConsoleW(consoleInput, outputArguments[currentIndex], BUFFER, &charactersRead, NULL);

    outputArguments[currentIndex][wcscspn(outputArguments[currentIndex], u"\r\n")] = u'\0';

    ++currentIndex;

    wprintf_s(u"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    outputArguments[currentIndex] = calloc(sizeof(u"-opts"), sizeof(char16_t));
    wcscpy_s(outputArguments[currentIndex], PATHBUF, u"-opts");
    ++currentIndex;

    outputArguments[currentIndex] = calloc(BUFFER, sizeof(char16_t));

    ReadConsoleW(consoleInput, outputArguments[currentIndex], BUFFER, &charactersRead, NULL);

    outputArguments[currentIndex][wcscspn(outputArguments[currentIndex], u"\r\n")] = u'\0';

    ++currentIndex;

    wprintf_s(u"%ls > %lsOutput extension: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    outputArguments[currentIndex] = calloc(sizeof(u"-ext"), sizeof(char16_t));
    wcscpy_s(outputArguments[currentIndex], PATHBUF, u"-ext");
    ++currentIndex;

    outputArguments[currentIndex] = calloc(SHORTBUF, sizeof(char16_t));

    ReadConsoleW(consoleInput, outputArguments[currentIndex], SHORTBUF, &charactersRead, NULL);
    
    outputArguments[currentIndex][wcscspn(outputArguments[currentIndex], u"\r\n")] = u'\0';

    ++currentIndex;

    wprintf_s(u"%ls > %lsAdditional flags: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    char16_t optionsString[BUFFER];

    ReadConsoleW(consoleInput, optionsString, BUFFER, &charactersRead, NULL);
    optionsString[wcscspn(optionsString, u"\r\n")] = u'\0';

    wchar_t *parserState = NULL;
    wchar_t *token = wcstok_s(optionsString, u", ", &parserState);
 
    while (token) {
        outputArguments[currentIndex] = calloc(SHORTBUF, sizeof(char16_t));
        wcscpy_s(outputArguments[currentIndex], SHORTBUF, token);
        token = wcstok_s(NULL, u", ", &parserState);
        ++currentIndex;
    }

    *outputArgumentsCount = currentIndex;

    wprintf_s(u"\n");
    wprintf_s(COLOR_DEFAULT);

    return outputArguments;
}

/* Parses the arguments after they've been properly formatted into a sequenced array of strings */
arguments_t parseCommandLineArguments(const int count, const char16_t *rawArguments[]) {
    arguments_t parsedArguments;

    for (size_t i = 0; i < count; ++i) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        if (wcscmp(rawArguments[i], u"-path") == 0) {
            wcsncpy_s(parsedArguments.inputPath, PATHBUF, rawArguments[++i], PATHBUF);
        } else if (wcscmp(rawArguments[i], u"-fmt") == 0) {
            wcsncpy_s(parsedArguments.inputFormatString, SHORTBUF, rawArguments[++i], BUFFER);
        } else if (wcscmp(rawArguments[i], u"-opts") == 0) {
            wcsncpy_s(parsedArguments.inputParameters, BUFFER, rawArguments[++i], BUFFER);
        } else if (wcscmp(rawArguments[i], u"-ext") == 0) {
            wcsncpy_s(parsedArguments.outputFormat, SHORTBUF, rawArguments[++i], SHORTBUF);
        }
    
        /* fmt: --help, --newfolder=foldername, --delete, --norecursion, --overwrite, */
        if (wcscmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArguments.optionDisplayHelp = true;
        } else if (wcscmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
            parsedArguments.optionDeleteOriginalFiles = true;
        } else if (wcscmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
            parsedArguments.optionDisableRecursiveSearch = true;
        } else if (wcscmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
            parsedArguments.optionForceFileOverwrites = true;
        } else if (wcsstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
            parsedArguments.optionMakeNewFolder = true;

            char16_t *argumentBuffer = wcsdup(rawArguments[i]); // duplicate argument string for analysis
            char16_t *parserState;
            char16_t *token = wcstok_s(argumentBuffer, u"=", &parserState);
            
            /* If there's an '=' sign, pass the string after it to the foldername argument */
            if ((token = wcstok_s(NULL, u"=", &parserState)) != NULL) {
                parsedArguments.optionCustomFolderName = true;
                wcscpy_s(parsedArguments.customFolderName, PATHBUF, token);
            }
            
            free(argumentBuffer);
        }
    }

    return parsedArguments;
}