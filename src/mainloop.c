#include "../include/mainloop.h"

int searchDirectory(const char16_t *directory, arguments *arguments, processInfo *runtimeData) {
    const char16_t *inputPath = directory == NULL ? arguments->inputPaths[0] : directory;
    size_t inputFormatIndex = 0;

    static char16_t *newFolderName = NULL;

    if (newFolderName == NULL)
        newFolderName = arguments->options & OPT_CUSTOMFOLDERNAME ? arguments->customFolderName : arguments->outputFormat;
    
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

        /* Avoid recursing into the brand new folder */
        if (wcscmp(fileName, newFolderName) == 0 && arguments->options & OPT_MAKENEWFOLDER)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && (arguments->options & OPT_DISABLERECURSION) == false) {
            char16_t newPathMask[PATHBUF];
            swprintf_s(newPathMask, PATHBUF, u"%ls\\%ls", inputPath, fileName);

            searchDirectory(newPathMask, arguments, runtimeData);

            continue;
        }

        bool isOfFormat = false;

        for (int i = 0; i < arguments->inputFormatsCount; i++) {
            if (wcsstr(fileName, arguments->inputFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (isOfFormat == false)
            continue;

        const char16_t *overwriteOption = arguments->options & OPT_FORCEFILEOVERWRITES ? u"-y" : u"";

        /* Copy filename except the extension */
        char16_t fileNameNoExtension[PATHBUF];
        wcsncpy_s(fileNameNoExtension, PATHBUF - 1, fileName, (wcslen(fileName) - wcslen(arguments->inputFormats[inputFormatIndex]) - 1));

        char16_t outputPath[PATHBUF];

        /* Make-a-subfolder-or-not part */
        if (arguments->options & OPT_MAKENEWFOLDER) {

            char16_t subFolderDirectory[PATHBUF];
            swprintf_s(subFolderDirectory, PATHBUF, u"%ls\\%ls", inputPath, newFolderName);

            CreateDirectoryW(subFolderDirectory, NULL);
            wcscpy_s(outputPath, PATHBUF, subFolderDirectory);
        } else {
            wcscpy_s(outputPath, PATHBUF, inputPath);
        }

        if ((arguments->options & OPT_FORCEFILEOVERWRITES) == false)
            preventFilenameOverwrites(fileNameNoExtension, arguments->outputFormat, outputPath);

        char16_t ffmpegProcessCall[LONGBUF];

        swprintf_s(ffmpegProcessCall, LONGBUF, u"ffmpeg -hide_banner %ls -i \"%ls\\%ls\" %ls \"%ls\\%ls.%ls\"", 
            overwriteOption, inputPath, fileName, arguments->ffmpegOptions, outputPath, fileNameNoExtension, arguments->outputFormat);

        /* Setup process info wcsuctures */
        STARTUPINFOW ffmpegStartupInformation = { sizeof(ffmpegStartupInformation) };
        PROCESS_INFORMATION ffmpegProcessInformation;

        /* Call ffmpeg and wait for it to finish */
        if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &ffmpegStartupInformation, &ffmpegProcessInformation)) {
            WaitForSingleObject(ffmpegProcessInformation.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInformation.hProcess);
            CloseHandle(ffmpegProcessInformation.hThread);
            runtimeData->convertedFiles++;

            wprintf_s(u"\n");
        }
        
        /* Keep or delete original files */
        if (arguments->options & OPT_DELETEORIGINALFILES) {
            char16_t inputFilePath[PATHBUF];
            swprintf_s(inputFilePath, PATHBUF, u"%ls\\%ls", arguments->inputPaths, fileName);

            if (DeleteFileW(inputFilePath)) {
                runtimeData->deletedFiles++;
            }
        }
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return NO_ERROR;
}