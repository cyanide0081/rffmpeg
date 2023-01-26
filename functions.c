#include "functions.h"

void printError(const wchar_t *msg) {
    fwprintf(stderr, L"%lsERROR: %ls%ls%ls\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

wchar_t **parseArguments(int count, const wchar_t *arguments[], wchar_t *destination[]) {
    /* fmt: -i <path> -f <container> -p <params> -o <container> */
    for (size_t i = 1; i < count; ++i) {
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
    for (size_t i = 0; i < count; ++i) {
        if (wcscmp(options[i], OPT_DISPLAYHELP_STRING) == 0) {
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
    if (arguments[ARG_INPATH] == NULL) { 
        arguments[ARG_INPATH] = malloc(PATHBUF);         
        GetCurrentDirectoryW(PATHBUF, arguments[ARG_INPATH]);
    }

    if (arguments[ARG_INPARAMETERS] == NULL) {
        arguments[ARG_INPARAMETERS] = malloc(2 * sizeof(wchar_t));
        wcscpy_s(arguments[ARG_INPARAMETERS], sizeof(wchar_t), L"");
    }

    if (arguments[ARG_INFORMAT] == NULL || *arguments[ARG_INFORMAT] == 0) {
        printError(L"no input format (null)");
        return EXIT_FAILURE;
    }

    if (arguments[ARG_OUTFORMAT] == NULL || *arguments[ARG_OUTFORMAT] == 0) {
        printError(L"no output format (null)");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int searchDirectory(const wchar_t *directory, wchar_t *arguments[], const bool options[], processInfo_t *processInformation) {
    const wchar_t *inputPath         = directory == NULL ? arguments[ARG_INPATH] : directory;
    const wchar_t *inputFormatString = arguments[ARG_INFORMAT];
    const wchar_t *parameters        = arguments[ARG_INPARAMETERS];
    const wchar_t *outputFormat      = arguments[ARG_OUTFORMAT];

    size_t inputFormatIndex = 0;
    static size_t numberOfInputFormats = 0;
    static wchar_t inputFormats[SHORTBUF][SHORTBUF] = { IDENTIFIER_NO_FORMAT };

    /* Save cpu resources by only tokenizing formats once */
    if (inputFormats[0] == IDENTIFIER_NO_FORMAT); {
        wchar_t inputFormatStringBuffer[BUFFER];

        wcscpy_s(inputFormatStringBuffer, BUFFER - 1, inputFormatString);

        wchar_t *parserState;
        wchar_t *token = wcstok_s(inputFormatStringBuffer, L", ", &parserState);

        while (token != NULL) {
            swprintf_s(inputFormats[numberOfInputFormats++], SHORTBUF - 1, L".%ls", token);
            token = wcstok_s(NULL, L", ", &parserState);
        }
    }
    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;
    wchar_t pathMask[BUFFER];

    swprintf_s(pathMask, BUFFER - 1, L"%ls\\*", inputPath);

    if ((fileHandle = FindFirstFileW(pathMask, &fileData)) == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"%lsERROR: %lsCouldn't open \'%ls\'\n", CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath);

        return EXIT_FAILURE;
    }

    do {
        /* Skip . and .. */
        if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && options[OPT_DISABLERECURSION] == false) {
            wchar_t newPathMask[PATHBUF];

            swprintf_s(newPathMask, PATHBUF - 1, L"%ls\\%ls", inputPath, fileData.cFileName);

            searchDirectory(newPathMask, arguments, options, processInformation);

            continue;
        }

        bool isOfFormat = false;

        for (size_t i = 0; i < numberOfInputFormats; ++i) {
            if (wcsstr(fileData.cFileName, inputFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (!isOfFormat)
            continue;

        wchar_t *fileName = fileData.cFileName;
        wchar_t fileNameNoExtension[PATHBUF];
        wchar_t outputPath[PATHBUF];

        const wchar_t *overwriteOption = options[OPT_FORCEOVERWRITE] == true ? L"-y" : L"";

        wcscpy_s(fileNameNoExtension, PATHBUF - 1, fileName);
        *(wcsstr(fileNameNoExtension, inputFormats[inputFormatIndex])) = L'\0'; 

        /* Make-a-subfolder-or-not part */
        if (options[OPT_MAKENEWFOLDER] == true) {
            wchar_t subFolderDirectory[PATHBUF];

            swprintf_s(subFolderDirectory, PATHBUF, L"%ls\\%ls", inputPath, outputFormat);
            CreateDirectoryW(subFolderDirectory, NULL);
            wcscpy_s(outputPath, PATHBUF - 1, subFolderDirectory);
        } else {
            wcscpy_s(outputPath, PATHBUF - 1, inputPath);
        }

        if (options[OPT_FORCEOVERWRITE] == false)
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, outputPath);

        wchar_t command[LONGBUF];

        swprintf_s(command, LONGBUF, L"ffmpeg -hide_banner %ls -i \"%ls\\%ls\" %ls \"%ls\\%ls.%ls\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);
        
        _wsystem(command);
        ++processInformation->convertedFiles;
        putchar('\n');

        /* Keep or delete original files */
        if (options[OPT_DELETEOLDFILES] == true) {
            wchar_t inputFilePath[PATHBUF];
            swprintf_s(inputFilePath, PATHBUF, L"%ls\\%ls", inputPath, fileName);

            if (DeleteFileW(inputFilePath)) {
                processInformation->deletedFiles++;
            }
        }
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}

void displayEndDialog(processInfo_t *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printError(L"No input files were found\n");
        wprintf_s(L"\n");
    } else {
        wprintf_s(L" %lsDONE!%ls\n", CHARCOLOR_RED, COLOR_DEFAULT);
        wprintf_s(L"\n");
        wprintf_s(L" %lsConverted file(s): %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->convertedFiles, COLOR_DEFAULT);
        wprintf_s(L" %lsDeleted file(s): %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->deletedFiles, COLOR_DEFAULT);
        wprintf_s(L" %lsExecution time: %ls%.2lfs%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->executionTime, COLOR_DEFAULT);
        wprintf_s(L"\n");
    }
 }