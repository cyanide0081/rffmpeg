#include "../include/search.h"

static int _searchDir(const char *directory, arguments *args, processInfo *runtimeData);

static bool _isDirectory(const char *dir);

/* runs _searchDir() in a list of directories */
int searchDirs(arguments *args, processInfo *runtimeData) {
    int code = EXIT_SUCCESS;

    for (int i = 0; args->inPaths[i] != NULL; i++) {
        code = _searchDir(args->inPaths[i], args, runtimeData);

        if (code != EXIT_SUCCESS)
            break;
    }

    return code;
}

/* Searches for files inside 'directory' and converts them with the given parameters */
static int _searchDir(const char *directory, arguments *args, processInfo *runtimeData) {
    const char *inputPath = directory;
    static char *newFolderName = NULL;

    if (newFolderName == NULL)
        newFolderName = args->options & OPT_CUSTOMFOLDERNAME ?
         args->customFolderName : args->outFormat;

    DIR *dir = opendir(inputPath);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        printError("couldn't open directory", inputPath);

        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        char *fileName = NULL;
        const char *inputFormat = NULL;

        #ifdef _WIN32
            size_t size = WideCharToMultiByte(CP_UTF8, 0, entry->d_name, -1, NULL, 0, NULL, NULL);
            fileName = xcalloc(size, sizeof(char));

            WideCharToMultiByte(CP_UTF8, 0, entry->d_name, -1, fileName, size, NULL, NULL);
        #else
            fileName = strdup(entry->d_name);
        #endif

        /* Skip . and .. */
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)
            continue;

        /* Avoid recursing into a brand new folder */
        if ((strcmp(fileName, newFolderName) == 0) && (args->options & OPT_NEWFOLDER))
            continue;

        char *fullInPath = asprintf("%s/%s", inputPath, fileName);

        /* Perform recursive search (or not) */
        if (_isDirectory(fullInPath) && !(args->options & OPT_NORECURSION)) {
            _searchDir(fullInPath, args, runtimeData);

            free(fullInPath);
            continue;
        }

        bool isOfFormat = false;

        for (int i = 0; args->inFormats[i]; i++) {
            if (strstr(fileName, args->inFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormat = args->inFormats[i];
                break;
            }
        }

        if (!isOfFormat) {
            free(fullInPath);
            continue;
        }

        /* Copy filename and remove the extension */
        char pureFileName[FILE_BUFFER];
        memccpy(pureFileName, fileName, '\0', FILE_BUFFER);
        memset(pureFileName + strlen(pureFileName) - strlen(inputFormat) - 1,
         '\0', strlen(inputFormat) + 1);

        char *outPath = NULL;

        /* Make-a-subfolder-or-not part */
        if (args->options & OPT_NEWFOLDER) {
            char *newPath = asprintf("%s/%s", inputPath, newFolderName);

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printError("couldn't create new directory", strerror(errno));

                return EXIT_FAILURE;
            }

            outPath = newPath;
        } else if (args->options & OPT_NEWPATH) {
            char *newPath = strdup(args->customPathName);

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printError("couldn't create new directory", strerror(errno));

                return EXIT_FAILURE;
            }

            outPath = newPath;
        } else {
            outPath = strdup(inputPath);
        }

        char *overwriteFlag = args->options & OPT_OVERWRITE ? strdup("-y") : strdup("");

        if (!(args->options & OPT_OVERWRITE))
            handleFileNameConflicts(pureFileName, args->outFormat, outPath);

        char *fullOutPath = asprintf("%s/%s.%s", outPath, pureFileName, args->outFormat);

        char *ffmpegCall = asprintf("ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
         overwriteFlag, fullInPath, args->ffOptions, fullOutPath);

        #ifdef _WIN32
            size_t callBuf = MultiByteToWideChar(CP_UTF8, 0, ffmpegCall, -1, NULL, 0);
            wchar_t *ffmpegCallW = xcalloc(callBuf, sizeof(wchar_t));

            MultiByteToWideChar(CP_UTF8, 0, ffmpegCall, -1, ffmpegCallW, callBuf);

            /* Setup process info structures */
            STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
            PROCESS_INFORMATION ffmpegProcessInfo;

            /* Call ffmpeg and wait for it to finish */
            bool createdProcess = CreateProcessW(NULL, ffmpegCallW, NULL,
            NULL, FALSE, 0, NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo);

            free(ffmpegCallW);

            if (createdProcess == false) {
                fwprintf_s(stderr, u"%lsERROR:%ls call to FFmpeg failed (code: %ls%lu%ls)\n\n",
                CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);
            } else {
                WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
                CloseHandle(ffmpegProcessInfo.hProcess);
                CloseHandle(ffmpegProcessInfo.hThread);

                runtimeData->convertedFiles++;
            }
        #else
            int systemCode = system(ffmpegCall);
            
            if (systemCode != EXIT_SUCCESS) {
                printError("call to FFmpeg failed", strerror(errno));
            } else {
                runtimeData->convertedFiles++;
            }
        #endif

        printf("\n");
        
        /* Keep or delete original files */
        if (args->options & OPT_CLEANUP) {
            if (remove(fullInPath) != 0) {
                printError("couldn't delete original file", strerror(errno));
            } else {
                runtimeData->deletedFiles++;
            }
        }

        /* Delete new folder in case it exists and no conversions succeeded */
        if (args->options & OPT_NEWFOLDER && runtimeData->convertedFiles == 0)
            remove(outPath);

        free(fileName);
        free(outPath);
        free(fullInPath);
        free(fullOutPath);
        free(ffmpegCall);
        free(overwriteFlag);
    }

    closedir(dir);

    return EXIT_SUCCESS;
}

#ifndef _WIN32
    static bool _isDirectory(const char *dir) {
        struct stat pathStats;
        stat(dir, &pathStats);

        return S_ISREG(pathStats.st_mode) == 0 ? true : false;
    }
#else
    static bool _isDirectory(const char *dir) {
        wchar_t dirW[PATH_BUFFER];
        MultiByteToWideChar(CP_UTF8, 0, dir, -1, dirW, PATH_BUFFER);

        DWORD fileAttr = GetFileAttributesW(dirW);

        if (fileAttr == INVALID_FILE_ATTRIBUTES)
            return false;
        if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
            return true;

        return false;
    }
#endif