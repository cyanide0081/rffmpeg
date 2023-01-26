#include "libs.h"

/* TODO:    implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:    get wchar arguments through windows function instead of wmain() to remove need for the compiler flag */
/* TODO:    maybe change wchar_t to char16_t to ensure a size of at least 16 bits per char */

int wmain(int argc, const wchar_t *argv[]) {
    wchar_t *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };
    processInfo_t processInformation = { 0 };

    int32_t exitCode;
    DWORD originalConsoleMode;
    clock_t startTime, endTime;

    char *locale = setlocale(LC_ALL, ".UTF-8");

    wchar_t originalConsoleWindowTitle[PATHBUF];

    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf(L"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, arguments);
        parseOptions(argc, argv, options);     
    } else {
        getInputFromConsole(arguments, options);
    }

    if (options[OPT_DISPLAYHELP] == true && inputMode == ARGUMENTS) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (handleErrors(arguments) == EXIT_FAILURE) {
        wprintf_s(L" %ls(Press any key to exit) %ls", CHARCOLOR_WHITE, COLOR_DEFAULT);

        getwchar();

        wprintf_s(L"\n");
        SetConsoleTitleW(originalConsoleWindowTitle);
        
        return EXIT_FAILURE;
    }

    startTime = clock();

    exitCode = searchDirectory(NULL, arguments, options, &processInformation);

    endTime = clock();

    processInformation.executionTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

    displayEndDialog(&processInformation);

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

    resetConsoleMode(originalConsoleMode);

    return exitCode;
}