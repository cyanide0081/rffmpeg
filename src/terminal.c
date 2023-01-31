#include "../include/terminal.h"

void printError(const char16_t  *msg) {
    fwprintf_s(stderr, u"%lsERROR: %ls%ls%ls\n\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

errno_t restoreConsoleMode(DWORD originalConsoleMode) {
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
        printError(u"No input files were found\n");
        wprintf_s(u"\n");
    } else {
        wprintf_s(u" %lsDONE!%ls\n", CHARCOLOR_RED, COLOR_DEFAULT);
        wprintf_s(u"\n");
        wprintf_s(u" %lsProcessed file(s): %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->convertedFiles, COLOR_DEFAULT);
        wprintf_s(u" %lsDeleted file(s): %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->deletedFiles, COLOR_DEFAULT);
        wprintf_s(u" %lsExecution time: %ls%.2lfs%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->executionTime, COLOR_DEFAULT);
        wprintf_s(u"\n");
    }
 }