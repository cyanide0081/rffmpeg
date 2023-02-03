#include "../include/libs.h"

int wmain(int argc, const char16_t *argv[]) {
    int exitCode = NO_ERROR;
    processInfo processInformation = { 0 };
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

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

    arguments *parsedArguments = calloc(1, sizeof(arguments));

    if (parsedArguments == NULL) {
        printError(u"not enough memory");

        return ERROR_NOT_ENOUGH_MEMORY;
    }

    if (inputMode == ARGUMENTS) {
        parseCommandLineArguments(argc, argv, parsedArguments);
    } else {
        parseArgumentsFromTerminal(parsedArguments);
    }

    if (parsedArguments->optionDisplayHelp == true && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(parsedArguments)) == NO_ERROR) {
        clock_t startTime = clock();
        exitCode = searchDirectory(NULL, parsedArguments, &processInformation);
        clock_t endTime = clock();

        if (exitCode == NO_ERROR) {
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