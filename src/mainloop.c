#include "../include/mainloop.h"

/* NOTE: This function performs various conversions between multi-byte and wide-char versions
 of strings because the WinAPI only supports Unicode in the 'W' variants of their functions */
errorCode_t searchDirectory(const char *directory, char *arguments[], const bool *options, processInfo_t *runtimeData) {
    const char *inputPath         = directory == NULL ? arguments[ARG_INPATH] : directory;
    const char *inputFormatString = arguments[ARG_INFORMAT];
    const char *parameters        = arguments[ARG_INPARAMETERS];
    const char *outputFormat      = arguments[ARG_OUTFORMAT];
    const char *newFolderName     = arguments[ARG_NEWFOLDERNAME];

    size_t inputFormatIndex = 0;
    static size_t numberOfInputFormats = 0;
    static char inputFormats[SHORTBUF][SHORTBUF] = { IDENTIFIER_NO_FORMAT };

    /* Save cpu resources by only tokenizing formats once */
    if (strcmp(inputFormats[0], IDENTIFIER_NO_FORMAT) == 0 && numberOfInputFormats == 0) {
        char inputFormatStringBuffer[BUFFER];

        strcpy_s(inputFormatStringBuffer, BUFFER - 1, inputFormatString);

        char *parserState;
        char *token = strtok_s(inputFormatStringBuffer, ", ", &parserState);

        while (token != NULL) {
            sprintf_s(inputFormats[numberOfInputFormats++], SHORTBUF - 1, ".%s", token);
            token = strtok_s(NULL, ", ", &parserState);
        }
    }
    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;
    wchar_t pathMaskWide[BUFFER];
    wchar_t inputPathWide[PATHBUF];
    
    MultiByteToWideChar(CP_UTF8, 0, inputPath, -1, inputPathWide, PATHBUF);
    swprintf_s(pathMaskWide, BUFFER, u"%ls\\*", inputPathWide);

    if ((fileHandle = FindFirstFileW(pathMaskWide, &fileData)) == INVALID_HANDLE_VALUE) {
        fprintf_s(stderr, "%sERROR: %sCouldn't open \'%s\' (code: %s%lu%s)\n\n",
            CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);

        return ERROR_FAILED_TO_OPEN_DIRECTORY;
    }

    do {
        char fileName[PATHBUF];
        WideCharToMultiByte(CP_UTF8, 0, fileData.cFileName, -1, fileName, PATHBUF, NULL, NULL);

        /* Skip . and .. */
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && options[OPT_DISABLERECURSION] == false) {
            char newPathMask[PATHBUF];
            sprintf_s(newPathMask, PATHBUF, "%s\\%s", inputPath, fileName);

            searchDirectory(newPathMask, arguments, options, runtimeData);

            continue;
        }

        bool isOfFormat = false;

        for (size_t i = 0; i < numberOfInputFormats; ++i) {
            if (strstr(fileName, inputFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (!isOfFormat)
            continue;

        char fileNameNoExtension[PATHBUF];
        char outputPath[PATHBUF];

        const char *overwriteOption = options[OPT_FORCEOVERWRITE] == true ? "-y" : "";

        strcpy_s(fileNameNoExtension, PATHBUF - 1, fileName);
        *(strstr(fileNameNoExtension, inputFormats[inputFormatIndex])) = L'\0'; 

        /* Make-a-subfolder-or-not part */
        if (options[OPT_MAKENEWFOLDER] == true) {
            const char *folderName = options[OPT_CUSTOMFOLDERNAME] == true ? arguments[ARG_NEWFOLDERNAME] : outputFormat;

            char subFolderDirectory[PATHBUF];
            wchar_t subFolderDirectoryWide[PATHBUF];
            sprintf_s(subFolderDirectory, PATHBUF, "%s\\%s", inputPath, folderName);
            MultiByteToWideChar(CP_UTF8, 0, subFolderDirectory, -1, subFolderDirectoryWide, PATHBUF);

            CreateDirectoryW(subFolderDirectoryWide, NULL);
            strcpy_s(outputPath, PATHBUF - 1, subFolderDirectory);
        } else {
            strcpy_s(outputPath, PATHBUF - 1, inputPath);
        }

        if (options[OPT_FORCEOVERWRITE] == false)
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, outputPath);

        char ffmpegProcessCall[LONGBUF];
        wchar_t ffmpegProcessCallWide[LONGBUF];

        sprintf_s(ffmpegProcessCall, LONGBUF, "ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s.%s\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);
        MultiByteToWideChar(CP_UTF8, 0, ffmpegProcessCall, -1, ffmpegProcessCallWide, PATHBUF);
        
        /* Setup process info structures */
        STARTUPINFOW ffmpegStartupInformation = { sizeof(ffmpegStartupInformation) };
        PROCESS_INFORMATION ffmpegProcessInformation;

        /* Call ffmpeg and wait for it to finish */
        if (CreateProcessW(NULL, ffmpegProcessCallWide, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &ffmpegStartupInformation, &ffmpegProcessInformation)) {
            WaitForSingleObject(ffmpegProcessInformation.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInformation.hProcess);
            CloseHandle(ffmpegProcessInformation.hThread);
            ++(runtimeData->convertedFiles);

            printf_s("\n");
        }
        
        /* Keep or delete original files */
        if (options[OPT_DELETEOLDFILES] == true) {
            char inputFilePath[PATHBUF];
            wchar_t inputFilePathWide[PATHBUF];

            sprintf_s(inputFilePath, PATHBUF, "%s\\%s", inputPath, fileName);
            MultiByteToWideChar(CP_UTF8, 0, inputFilePath, -1, inputFilePathWide, PATHBUF);

            if (DeleteFileW(inputFilePathWide)) {
                ++(runtimeData->deletedFiles);
            }
        }
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}