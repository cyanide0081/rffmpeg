#include "libs.h"

/* TODO:    implement custom newfolder naming (--newfolder=foldername) */
/* TODO:    (maybe) implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:    get wchar arguments through windows function instead of wmain() to remove need for the compiler flag */
/* TODO:    maybe change wchar_t to char16_t to ensure a size of at least 16 bits per char */

int wmain(int argc, const wchar_t *argv[]) {
    wchar_t *arguments[MAX_ARGS];

    arguments[ARG_INPATH]           = calloc(PATHBUF, sizeof(wchar_t));
    arguments[ARG_INFORMAT]         = calloc(BUFFER, sizeof(wchar_t));
    arguments[ARG_INPARAMETERS]     = calloc(BUFFER, sizeof(wchar_t));
    arguments[ARG_OUTFORMAT]        = calloc(SHORTBUF, sizeof(wchar_t));
    arguments[ARG_NEWFOLDERNAME]    = calloc(PATHBUF, sizeof(wchar_t));

    bool options[MAX_OPTS] = { false };

    processInfo_t processInformation = { 0 };
    errorCode_t exitCode = ERROR_NONE;
    DWORD originalConsoleMode;
    wchar_t originalConsoleWindowTitle[PATHBUF];

    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    char *originalLocale = strdup(setlocale(LC_ALL, NULL));
    
    setlocale(LC_ALL, ".UTF-8");
    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf_s(L"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, arguments, options, true, true);
    } else {
        getInputFromConsole(arguments, options);
    }

    if (options[OPT_DISPLAYHELP] == true && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(arguments)) == ERROR_NONE) {
        clock_t startTime = clock();

        exitCode = searchDirectory(NULL, arguments, options, &processInformation);

        clock_t endTime = clock();

        if (exitCode == ERROR_NONE) {
            processInformation.executionTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

            displayEndDialog(&processInformation);
        }
    }

    /* Free allocated pointers */
    for (size_t i = 0; i < MAX_ARGS; ++i) {        
        free(arguments[i]);
        arguments[i] = NULL;    
    }

    if (inputMode == CONSOLE) {
        wprintf_s(L" %ls(Press any key to exit) %ls", CHARCOLOR_WHITE, COLOR_DEFAULT);

        getwchar();

        wprintf_s(L"\n");
        SetConsoleTitleW(originalConsoleWindowTitle);
    }

    setlocale(LC_ALL, originalLocale);
    free(originalLocale);

    resetConsoleMode(originalConsoleMode);

    return (int)exitCode;
}