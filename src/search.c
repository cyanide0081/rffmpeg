#include "../include/search.h"

int searchDirectory(const char16_t *directory, arguments *arguments, processInfo *runtimeData) {
    const char16_t *inputPath = directory == NULL ? arguments->inPaths[0] : directory;
    size_t inputFormatIndex = 0;

    static char16_t *newFolderName = NULL;

    if (newFolderName == NULL)
        newFolderName = arguments->options & OPT_CUSTOMFOLDERNAME ?
         arguments->customFolderName : arguments->outFormat;
    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;
    char16_t pathMask[PATH_BUFFER];

    swprintf_s(pathMask, PATH_BUFFER, u"%ls\\*", inputPath);

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
        if (wcscmp(fileName, newFolderName) == 0 && arguments->options & OPT_NEWFOLDER)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && (arguments->options
         & OPT_NORECURSION) == false) {
            char16_t newPathMask[PATH_BUFFER];
            swprintf_s(newPathMask, PATH_BUFFER, u"%ls\\%ls", inputPath, fileName);

            searchDirectory(newPathMask, arguments, runtimeData);

            continue;
        }

        bool isOfFormat = false;

        for (int i = 0; i < arguments->inFormatsCount; i++) {
            if (wcsstr(fileName, arguments->inFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (isOfFormat == false)
            continue;

        const char16_t *overwriteOption = arguments->options & OPT_OVERWRITE ? u"-y" : u"";

        /* Copy filename except the extension */
        char16_t pureFileName[PATH_BUFFER];
        wcsncpy_s(pureFileName, PATH_BUFFER - 1, fileName,
         (wcslen(fileName) - wcslen(arguments->inFormats[inputFormatIndex]) - 1));

        char16_t outputPath[PATH_BUFFER];
        char16_t newFolderPath[PATH_BUFFER];

        /* Make-a-subfolder-or-not part */
        if (arguments->options & OPT_NEWFOLDER) {
            swprintf_s(newFolderPath, PATH_BUFFER, u"%ls\\%ls", inputPath, newFolderName);

            CreateDirectoryW(newFolderPath, NULL);
            wcscpy_s(outputPath, PATH_BUFFER, newFolderPath);
        } else {
            wcscpy_s(outputPath, PATH_BUFFER, inputPath);
        }

        if ((arguments->options & OPT_OVERWRITE) == false)
            preventFilenameOverwrites(pureFileName, arguments->outFormat, outputPath);

        char16_t ffmpegProcessCall[LONGBUF];

        swprintf_s(ffmpegProcessCall, LONGBUF,
         u"ffmpeg -hide_banner %ls -i \"%ls\\%ls\" %ls \"%ls\\%ls.%ls\"", overwriteOption, inputPath,
         fileName, arguments->ffOptions, outputPath, pureFileName, arguments->outFormat);

        /* Setup process info wcsuctures */
        STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
        PROCESS_INFORMATION ffmpegProcessInfo;

        /* Call ffmpeg and wait for it to finish */
        bool createdProcess = CreateProcessW(NULL, ffmpegProcessCall, NULL,
         NULL, FALSE, 0, NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo);

        if (createdProcess == false) {
            fwprintf_s(stderr, u"%lsERROR:%ls call to FFmpeg failed (code: %ls%lu%ls)\n\n",
             CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);

            continue;
        }
        
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);

        runtimeData->convertedFiles++;

        wprintf_s(u"\n");
        
        /* Keep or delete original files */
        if (arguments->options & OPT_CLEANUP) {
            char16_t inputFilePath[PATH_BUFFER];
            swprintf_s(inputFilePath, PATH_BUFFER, u"%ls\\%ls", arguments->inPaths, fileName);

            if (DeleteFileW(inputFilePath))
                runtimeData->deletedFiles++;
        }

        /* Delete new folder in case it exists and no conversions succeeded */
        if (arguments->options & OPT_NEWFOLDER && runtimeData->convertedFiles == 0)
            DeleteFileW(newFolderPath);
    } while (FindNextFileW(fileHandle, &fileData));

    FindClose(fileHandle);

    return NO_ERROR;
}