#include "../include/terminal.h"

void printError(const char *msg) {
    fprintf(stderr, "%sERROR: %s%s%s\n\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

errno_t clearConsoleWindow(void) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD consoleMode = 0;

    if (!GetConsoleMode(handleToStdOut, &consoleMode)) {
        return GetLastError();
    }

    const DWORD originalConsoleMode = consoleMode;

    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(handleToStdOut, consoleMode)) {
        return GetLastError();
    }

    DWORD writtenCharacters = 0;
    PCSTR sequence = "\x1b]10d\x1b]1G";

    if (!WriteConsoleA(handleToStdOut, sequence, (DWORD)strlen(sequence), &writtenCharacters, NULL)) {
        SetConsoleMode(handleToStdOut, originalConsoleMode);
        return GetLastError();
    }

    SetConsoleMode(handleToStdOut, originalConsoleMode);

    return EXIT_SUCCESS;
}

errno_t resetConsoleMode(DWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!SetConsoleMode(handleToStdOut, originalConsoleMode)) {
        return GetLastError();
    }

    return EXIT_SUCCESS;
}

errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD consoleMode = 0;

    if (!GetConsoleMode(handleToStdOut, &consoleMode)) {
        return GetLastError();
    }

    *originalConsoleMode = consoleMode;

    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(handleToStdOut, consoleMode)) {
        return GetLastError();
    }

    return EXIT_SUCCESS;
}

void displayEndDialog(processInfo_t *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printError("No input files were found\n");
        printf_s("\n");
    } else {
        printf_s(" %sDONE!%s\n", CHARCOLOR_RED, COLOR_DEFAULT);
        printf_s("\n");
        printf_s(" %sConverted file(s): %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->convertedFiles, COLOR_DEFAULT);
        printf_s(" %sDeleted file(s): %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->deletedFiles, COLOR_DEFAULT);
        printf_s(" %sExecution time: %s%.2lfs%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->executionTime, COLOR_DEFAULT);
        printf_s("\n");
    }
 }