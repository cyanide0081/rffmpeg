#include "libs.h"

/* TODO:    implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:    get wchar arguments through windows function instead of wmain() to remove need for the compiler flag */

int wmain(int argc, const wchar_t *argv[]) {
    wchar_t *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };
    processInfo_t processInformation = { 0 };

    int32_t exitCode;
    clock_t startTime, endTime;
    inputMode_t inputMode;

    SetConsoleCP(_utf8Codepage);
    SetConsoleOutputCP(_utf8Codepage);

    wprintf(L"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (argc > 1) {
        parseArguments(argc, argv, arguments);
        parseOptions(argc, argv, options);     
        inputMode = ARGUMENTS;
    } else {
        runInConsoleMode(arguments, options);
        inputMode = CONSOLE;
    }

    if (options[OPT_DISPLAYHELP] == true) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (handleErrors(arguments) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    startTime = clock();

    exitCode = searchDirectory(NULL, arguments, (const bool*)options, &processInformation);

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
    }

    return exitCode;
}