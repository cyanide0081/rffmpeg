#include "../include/input.h"

/* Gets and parses the argument strings from console input dialogs */
errno_t parseArgumentsFromTerminal(arguments_t *arguments) {
    size_t currentIndex = 0;
    DWORD charactersRead = 0;
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);

    wprintf_s(u"%ls > %lsInput path: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->inputPath, PATHBUF, &charactersRead, NULL);
    removeTrailingNewLine(arguments->inputPath);

    wprintf_s(u"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->inputFormatString, SHORTBUF, &charactersRead, NULL);
    removeTrailingNewLine(arguments->inputFormatString);

    wprintf_s(u"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->inputParameters, BUFFER, &charactersRead, NULL);
    removeTrailingNewLine(arguments->inputParameters);

    wprintf_s(u"%ls > %lsOutput extension: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->outputFormat, SHORTBUF, &charactersRead, NULL);
    removeTrailingNewLine(arguments->outputFormat);

    char16_t optionsString[BUFFER];

    wprintf_s(u"%ls > %lsAdditional flags: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, optionsString, BUFFER, &charactersRead, NULL);
    removeTrailingNewLine(optionsString);


    wchar_t *parserState = NULL;
    wchar_t *token = wcstok_s(optionsString, u", ", &parserState);

    char16_t *optionsList[SHORTBUF] = { NULL };
    size_t optionsCount = 0;
 
    for (int i = 0; token != NULL; optionsCount = ++i) {
        if ((optionsList[i] = calloc(SHORTBUF, sizeof(char16_t))) == NULL) {
            printError(u"not enough memory");

            return ERROR_NOT_ENOUGH_MEMORY;
        }

        wcscpy_s(optionsList[i], SHORTBUF, token);
        token = wcstok_s(NULL, u", ", &parserState);
    }

    parseCommandLineArguments(optionsCount, (const char16_t**)optionsList, arguments);

    for (int i = 0; i < optionsCount; ++i) {
        free(optionsList[i]);
        optionsList[i] = NULL;
    }

    wprintf_s(u"\n");
    wprintf_s(COLOR_DEFAULT);

    return NO_ERROR;
}

/* Parses an array of strings to format parsedArguments accordingly */
errno_t parseCommandLineArguments(const int count, const char16_t *rawArguments[], arguments_t *parsedArguments) {

    for (int i = 0; i < count; ++i) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        if (wcscmp(rawArguments[i], u"-path") == 0) {
            wcsncpy_s(parsedArguments->inputPath, PATHBUF, rawArguments[++i], PATHBUF);
        } else if (wcscmp(rawArguments[i], u"-fmt") == 0) {
            wcsncpy_s(parsedArguments->inputFormatString, SHORTBUF, rawArguments[++i], BUFFER);
        } else if (wcscmp(rawArguments[i], u"-opts") == 0) {
            wcsncpy_s(parsedArguments->inputParameters, BUFFER, rawArguments[++i], BUFFER);
        } else if (wcscmp(rawArguments[i], u"-ext") == 0) {
            wcsncpy_s(parsedArguments->outputFormat, SHORTBUF, rawArguments[++i], SHORTBUF);
        }
    
        /* fmt: --help, --newfolder=foldername, --delete, --norecursion, --overwrite, */
        if (wcscmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArguments->optionDisplayHelp = true;
        } else if (wcscmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
            parsedArguments->optionDeleteOriginalFiles = true;
        } else if (wcscmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
            parsedArguments->optionDisableRecursiveSearch = true;
        } else if (wcscmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
            parsedArguments->optionForceFileOverwrites = true;
        } else if (wcsstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
            parsedArguments->optionMakeNewFolder = true;

            char16_t *argumentBuffer = wcsdup(rawArguments[i]); // duplicate argument string for analysis
            char16_t *parserState;
            char16_t *token = wcstok_s(argumentBuffer, u"=", &parserState);
            
            /* If there's an '=' sign, pass the string after it to the foldername argument */
            if ((token = wcstok_s(NULL, u"=", &parserState)) != NULL) {
                parsedArguments->optionCustomFolderName = true;
                wcscpy_s(parsedArguments->customFolderName, PATHBUF, token);
            }
            
            free(argumentBuffer);
        }
    }

    return NO_ERROR;
}