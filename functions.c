#include "functions.h"

void printError(const wchar_t *msg) {
    fwprintf(stderr, L"%lsERROR: %ls%ls%ls\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

wchar_t **parseArguments(int count, const wchar_t *arguments[], wchar_t *destination[]) {
    /* fmt: -i <path> -f <container> -p <params> -o <container> */
    for (int i = 1; i < count - 1; ++i) {
        if (wcscmp(arguments[i], L"-i") == 0) {
            destination[ARG_INPATH] = malloc(PATHBUF);
            wcsncpy_s(destination[ARG_INPATH], MAX_PATH - 1, arguments[++i], MAX_PATH);
        } else if (wcscmp(arguments[i], L"-f") == 0) {
            destination[ARG_INFORMAT] = malloc(BUFFER);
            wcsncpy_s(destination[ARG_INFORMAT], BUFFER - 1, arguments[++i], BUFFER);
        } else if (wcscmp(arguments[i], L"-p") == 0) {
            destination[ARG_INPARAMETERS] = malloc(BUFFER);
            wcsncpy_s(destination[ARG_INPARAMETERS], BUFFER - 1, arguments[++i], BUFFER);
        } else if (wcscmp(arguments[i], L"-o") == 0) {
            destination[ARG_OUTFORMAT] = malloc(SHORTBUF);
            wcsncpy_s(destination[ARG_OUTFORMAT], SHORTBUF - 1, arguments[++i], SHORTBUF);
        }
    }

    return destination;
}

bool *parseOptions(int count, const wchar_t *options[], bool destination[]) {
    /* fmt: --help (duh) /n (folder) /d (delete old) /r (recursive) /y (overwrite) */
    for (int i = 1; i < count; ++i) {
        if (wcsstr(options[i], OPT_DISPLAYHELP_STRING)) {
            destination[OPT_DISPLAYHELP] = true;
        } else if (wcscmp(options[i], OPT_MAKENEWFOLDER_STRING) == 0) {
            destination[OPT_MAKENEWFOLDER] = true;
        } else if (wcscmp(options[i], OPT_DELETEOLDFILES_STRING) == 0) {
            destination[OPT_DELETEOLDFILES] = true;
        } else if (wcscmp(options[i], OPT_DISABLERECURSION_STRING) == 0) {
            destination[OPT_DISABLERECURSION] = true;
        } else if (wcscmp(options[i], OPT_FORCEOVERWRITE_STRING) == 0) {
            destination[OPT_FORCEOVERWRITE] = true;
        }
    }

    return destination;
}

int preventFilenameOverwrites(wchar_t *pureFilename, const wchar_t *outputFormat, const wchar_t *path) {
    wchar_t fileMask[PATHBUF];
    wchar_t fileNameNew[PATHBUF];

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;

    swprintf_s(fileMask, PATHBUF, L"%ls\\%ls.%ls", path, pureFilename, outputFormat);

    if ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            swprintf_s(fileMask, BUFFER, L"%ls\\%ls-%03d.%ls", path, pureFilename, ++index, outputFormat);
        }   while ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE);

        swprintf_s(fileNameNew, PATHBUF, L"%ls-%03d", pureFilename, index);
        wcscpy_s(pureFilename, PATHBUF, fileNameNew);
    }

    return EXIT_SUCCESS;
}

int handleErrors(wchar_t *arguments[]) {  
    if (!arguments[ARG_INPATH] || *arguments[ARG_INPATH] == 0) { 
        arguments[ARG_INPATH] = malloc(PATHBUF);         
        GetCurrentDirectoryW(PATHBUF, arguments[ARG_INPATH]);
    }

    if (!arguments[ARG_INPARAMETERS] || *arguments[ARG_INPARAMETERS] == 0) {
        arguments[ARG_INPARAMETERS] = L"";
    }

    if (!arguments[ARG_INFORMAT] || *arguments[ARG_INFORMAT] == 0) {
        printError(L"no input format (null)");
        return EXIT_FAILURE;
    }

    if (!arguments[ARG_OUTFORMAT] || *arguments[ARG_OUTFORMAT] == 0) {
        printError(L"no output format (null)");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}