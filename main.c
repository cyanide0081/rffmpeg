#include "libs.h"

/* TODO:  add new folder option and delete-old-files option */
/* TODO:  implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:  add dialogues after processing stage */

int processFiles(const wchar_t *directory, const wchar_t *arguments[], const bool options[]);

int wmain(int argc, const wchar_t *argv[]) {
    wchar_t *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };
    int exitCode;

    SetConsoleCP(_utf8Codepage);
    SetConsoleOutputCP(_utf8Codepage);

    wprintf(L"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (argc > 1) {
        parseArguments(argc, argv, arguments);
        parseOptions(argc, argv, options);
    } else {
        runInConsoleMode(arguments, options);
    }

    exitCode = processFiles(NULL, (const wchar_t**)arguments, (const bool*)options);

    for (int i = 0; arguments[i]; ++i)
        free(arguments[i]);

    putwchar(L'\n');

    return exitCode;
}

int processFiles(const wchar_t *directory, const wchar_t *arguments[], const bool options[]) {
    if (options[OPT_DISPLAYHELP]) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (handleErrors((wchar_t**)arguments))
        return EXIT_FAILURE;

    const wchar_t *inputPath          = arguments[ARG_INPATH];
    const wchar_t *inputFormatString  = arguments[ARG_INFORMAT];
    const wchar_t *parameters         = arguments[ARG_INPARAMETERS];
    const wchar_t *outputFormat       = arguments[ARG_OUTFORMAT];

    if (directory == NULL)
        inputPath = arguments[ARG_INPATH];
    else
        inputPath = directory;

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

        /* Recursive part */
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

        if (options[OPT_FORCEOVERWRITE] == true)
            overwriteOption = L"-y";
        else
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, inputPath);

        /* temporary output path hardcode to input path */
        wcscpy_s(outputPath, sizeof outputPath, inputPath);

        wchar_t command[LONGBUF];

        swprintf_s(command, LONGBUF, L"ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s.%s\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);
        
        _wsystem(command);     
        putchar('\n');

    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}