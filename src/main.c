#include "../include/libs.h"
#include "../include/headers.h"

/* TODO: restructure the whole code to make use of portable I/O
 functions and become ready for compiling for Unix-like OSes */

int wmain(int argc, char16_t *argv[]) {
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT); // Setup Unicode (UTF-16LE) console I/O for Windows
    #endif
    
    int exitCode = EXIT_SUCCESS;
    processInfo processInformation = { 0 };
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    /* Enable virtual terminal sequences for colored console output */
    DWORD originalConsoleMode;
    char16_t originalConsoleWindowTitle[PATH_BUFFER];
    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATH_BUFFER);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf_s(u"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);
    
    arguments *parsedArguments = initializeArguments();

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, parsedArguments);
    } else {
        parseConsoleInput(parsedArguments);
    }

    if (parsedArguments->options & OPT_DISPLAYHELP && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(parsedArguments)) == EXIT_SUCCESS) {
        clock_t startTime = clock();

        /* TODO: remove this loop later (ugly way of looping through paths) */
        for (int i = 0; i < parsedArguments->inputPathsCount; i++)
            exitCode = searchDirectory(parsedArguments->inputPaths[i], parsedArguments, &processInformation);

        clock_t endTime = clock();

        if (exitCode == EXIT_SUCCESS) {
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

    destroyArguments(parsedArguments);
    restoreConsoleMode(originalConsoleMode);

    return exitCode;
}