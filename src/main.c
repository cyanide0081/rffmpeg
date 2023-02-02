#include "../include/libs.h"

int wmain(int argc, const char16_t *argv[]) {
    errorCode_t exitCode = ERROR_NONE;
    processInfo_t processInformation = { 0 };
    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    _setmode(_fileno(stdout), _O_U16TEXT); // Setup Unicode (UTF-16LE) console I/O

    /* Enable virtual terminal sequences for colored console output */
    DWORD originalConsoleMode;
    char16_t originalConsoleWindowTitle[PATHBUF];
    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf_s(u"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    arguments_t *parsedArguments = malloc(sizeof(arguments_t));

    if (inputMode == ARGUMENTS) {
        *parsedArguments = parseCommandLineArguments(argc, argv);
    } else {
        size_t consoleArgumentsCount = 0;
        char16_t *consoleArguments[SHORTBUF];

        parseArgumentsFromTerminal(&consoleArgumentsCount, consoleArguments);

        *parsedArguments = parseCommandLineArguments(consoleArgumentsCount, (const char16_t**)consoleArguments);

        for (size_t i = 0; i < consoleArgumentsCount; ++i) {
            free(consoleArguments[i]);
            consoleArguments[i] = NULL;
        }
    }

    if (parsedArguments->optionDisplayHelp == true && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(parsedArguments)) == ERROR_NONE) {
        clock_t startTime = clock();
        exitCode = searchDirectory(NULL, parsedArguments, &processInformation);
        clock_t endTime = clock();

        if (exitCode == ERROR_NONE) {
            processInformation.executionTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            
            displayEndDialog(&processInformation);
        }
    }

    if (inputMode == CONSOLE) {
        wprintf_s(u" %ls(Press any key to exit) %ls", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getwchar();

        wprintf_s(u"\n");
        SetConsoleTitleW(originalConsoleWindowTitle);
    }

    free(parsedArguments);
    restoreConsoleMode(originalConsoleMode);

    return (int)exitCode;
}