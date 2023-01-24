#include "libs.h"

/* TODO:  implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:  add dialogues after processing stage */
/* TODO:  experiment with making some strings static inside processFiles() to conserve more memory */

int processFiles(const wchar_t *directory, wchar_t *arguments[], const bool options[]);

int wmain(int argc, const wchar_t *argv[]) {
    wchar_t *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };

    SetConsoleCP(_utf8Codepage);
    SetConsoleOutputCP(_utf8Codepage);

    wprintf(L"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (argc > 1) {
        parseArguments(argc, argv, arguments);
        parseOptions(argc, argv, options);
    } else {
        runInConsoleMode(arguments, options);
    }

    int32_t exitCode = processFiles(NULL, arguments, (const bool*)options);

    for (int i = 0; arguments[i]; ++i)
        free(arguments[i]);

    putwchar(L'\n');

    return exitCode;
}

int processFiles(const wchar_t *directory, wchar_t *arguments[], const bool options[]) {
    if (options[OPT_DISPLAYHELP] == true) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (handleErrors(arguments))
        return EXIT_FAILURE;

    static size_t numberOfConvertedFiles = 0;

    const wchar_t *inputPath         = directory == NULL ? arguments[ARG_INPATH] : directory;
    const wchar_t *inputFormatString = arguments[ARG_INFORMAT];
    const wchar_t *parameters        = arguments[ARG_INPARAMETERS];
    const wchar_t *outputFormat      = arguments[ARG_OUTFORMAT];


    /* Tokenize input formats */
    wchar_t inputFormats[SHORTBUF][SHORTBUF];
    size_t numberOfInputFormats = 0;
    size_t inputFormatIndex = 0;
    wchar_t *savePointer;
    wchar_t *token = wcstok_s((wchar_t*)inputFormatString, L", ", &savePointer);

    while (token != NULL) {
        swprintf_s(inputFormats[numberOfInputFormats++], SHORTBUF - 1, L".%ls", token);
        token = wcstok_s(NULL, L", ", &savePointer);
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

            processFiles(newPathMask, arguments, options);

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
        wchar_t *overwriteOption = L"";

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

        /* Check for possible filename overwrites */
        if (options[OPT_FORCEOVERWRITE] == true)
            overwriteOption = L"-y";
        else
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, outputPath);

        wchar_t command[LONGBUF];

        swprintf_s(command, LONGBUF, L"ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s.%s\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);
        
        _wsystem(command);
        putchar('\n');

        /* Keep or delete old files */
        if (options[OPT_DELETEOLDFILES] == true) {
            wchar_t inputFilePath[PATHBUF];
            swprintf_s(inputFilePath, PATHBUF, L"%ls\\%ls", inputPath, fileName);

            DeleteFileW(inputFilePath);
        }
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}