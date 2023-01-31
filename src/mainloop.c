#include "../include/mainloop.h"

errorCode_t searchDirectory(const char16_t *directory, char16_t *arguments[], const bool *options, processInfo_t *runtimeData) {
    const char16_t *inputPath         = directory == NULL ? arguments[ARG_INPATH] : directory;
    const char16_t *inputFormatString = arguments[ARG_INFORMAT];
    const char16_t *parameters        = arguments[ARG_INPARAMETERS];
    const char16_t *outputFormat      = arguments[ARG_OUTFORMAT];
    const char16_t *newFolderName     = arguments[ARG_NEWFOLDERNAME];

    size_t inputFormatIndex = 0;
    static size_t numberOfInputFormats = 0;
    static char16_t inputFormats[SHORTBUF][SHORTBUF] = { IDENTIFIER_NO_FORMAT };

    /* Save cpu resources by only tokenizing formats once */
    if (wcscmp(inputFormats[0], IDENTIFIER_NO_FORMAT) == 0 && numberOfInputFormats == 0) {
        char16_t inputFormatStringBuffer[BUFFER];

        wcscpy_s(inputFormatStringBuffer, BUFFER - 1, inputFormatString);

        char16_t *parserState;
        char16_t *token = wcstok_s(inputFormatStringBuffer, u", ", &parserState);

        while (token != NULL) {
            swprintf_s(inputFormats[numberOfInputFormats++], SHORTBUF - 1, u".%ls", token);
            token = wcstok_s(NULL, u", ", &parserState);
        }
    }
    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;
    char16_t pathMask[BUFFER];

    swprintf_s(pathMask, BUFFER, u"%ls\\*", inputPath);

    if ((fileHandle = FindFirstFileW(pathMask, &fileData)) == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, u"%lsERROR: %lsCouldn't open \'%ls\' (code: %ls%lu%ls)\n\n",
            CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);

        return ERROR_FAILED_TO_OPEN_DIRECTORY;
    }

    do {
        char16_t *fileName = fileData.cFileName;

        /* Skip . and .. */
        if (wcscmp(fileName, u".") == 0 || wcscmp(fileName, u"..") == 0)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && options[OPT_DISABLERECURSION] == false) {
            char16_t newPathMask[PATHBUF];
            swprintf_s(newPathMask, PATHBUF, u"%ls\\%ls", inputPath, fileName);

            searchDirectory(newPathMask, arguments, options, runtimeData);

            continue;
        }

        bool isOfFormat = false;

        for (size_t i = 0; i < numberOfInputFormats; ++i) {
            if (wcsstr(fileName, inputFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (!isOfFormat)
            continue;

        char16_t fileNameNoExtension[PATHBUF];
        char16_t outputPath[PATHBUF];

        const char16_t *overwriteOption = options[OPT_FORCEOVERWRITE] == true ? u"-y" : u"";

        wcscpy_s(fileNameNoExtension, PATHBUF - 1, fileName);
        *(wcsstr(fileNameNoExtension, inputFormats[inputFormatIndex])) = u'\0'; 

        /* Make-a-subfolder-or-not part */
        if (options[OPT_MAKENEWFOLDER] == true) {
            const char16_t *folderName = options[OPT_CUSTOMFOLDERNAME] == true ? newFolderName : outputFormat;

            char16_t subFolderDirectory[PATHBUF];
            swprintf_s(subFolderDirectory, PATHBUF, u"%ls\\%ls", inputPath, folderName);

            CreateDirectoryW(subFolderDirectory, NULL);
            wcscpy_s(outputPath, PATHBUF, subFolderDirectory);
        } else {
            wcscpy_s(outputPath, PATHBUF, inputPath);
        }

        if (options[OPT_FORCEOVERWRITE] == false)
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, outputPath);

        char16_t ffmpegProcessCall[LONGBUF];

        swprintf_s(ffmpegProcessCall, LONGBUF, u"ffmpeg -hide_banner %ls -i \"%ls\\%ls\" %ls \"%ls\\%ls.%ls\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);

        /* Setup process info wcsuctures */
        STARTUPINFOW ffmpegStartupInformation = { sizeof(ffmpegStartupInformation) };
        PROCESS_INFORMATION ffmpegProcessInformation;

        /* Call ffmpeg and wait for it to finish */
        if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &ffmpegStartupInformation, &ffmpegProcessInformation)) {
            WaitForSingleObject(ffmpegProcessInformation.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInformation.hProcess);
            CloseHandle(ffmpegProcessInformation.hThread);
            ++(runtimeData->convertedFiles);

            wprintf_s(u"\n");
        }
        
        /* Keep or delete original files */
        if (options[OPT_DELETEOLDFILES] == true) {
            char16_t inputFilePath[PATHBUF];
            swprintf_s(inputFilePath, PATHBUF, u"%ls\\%ls", inputPath, fileName);

            if (DeleteFileW(inputFilePath)) {
                ++(runtimeData->deletedFiles);
            }
        }
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}