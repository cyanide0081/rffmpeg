#include "../include/search.h"

static int _searchDirectory(const char *directory, arguments *args, processInfo *runtimeData);

static bool _isDirectory(const char *pathToFile);

int searchDirectories(arguments *args, processInfo *runtimeData) {
    int code;

    for (int i = 0; args->inPaths[i] != NULL; i++) {
        code = _searchDirectory(args->inPaths[i], args, runtimeData);
    }

    return code;
}

static int _searchDirectory(const char *directory, arguments *args, processInfo *runtimeData) {
    const char *inputPath = directory;
    static char *newFolderName = NULL;

    if (newFolderName == NULL)
        newFolderName = args->options & OPT_CUSTOMFOLDERNAME ?
         args->customFolderName : args->outFormat;

    DIR *dir = opendir(inputPath);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        printError("couldn't open directory", strerror(errno));

        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        const char *fileName = entry->d_name;
        const char *inputFormat = NULL;

        /* Skip . and .. */
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)
            continue;

        /* Avoid recursing into a brand new folder */
        if ((strcmp(fileName, newFolderName) == 0) && (args->options & OPT_NEWFOLDER))
            continue;

        char *fullInPath = asprintf("%s/%s", inputPath, fileName);

        /* Perform recursive search (or not) */
        if (_isDirectory(fullInPath) && !(args->options & OPT_NORECURSION)) {
            _searchDirectory(fullInPath, args, runtimeData);

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
            const char *newFolder = (args->options & OPT_CUSTOMFOLDERNAME) ?
             args->customFolderName : inputFormat;

            char *newPath = asprintf("%s/%s", inputPath, newFolder);

            mkdir(newPath, S_IRWXU);

            outPath = newPath;
        } else if (args->options & OPT_NEWPATH) {
            char *newPath = strdup(args->customPathName);

            mkdir(newPath, S_IRWXU);

            outPath = newPath;
        } else {
            outPath = strdup(inputPath);
        }

        char *overwriteFlag = args->options & OPT_OVERWRITE ? strdup("-y") : strdup("");

        if (!(args->options & OPT_OVERWRITE))
            preventFilenameOverwrites(pureFileName, args->outFormat, outPath);

        char *fullOutPath = asprintf("%s/%s.%s", outPath, pureFileName, args->outFormat);

        char *ffmpegCall = asprintf("ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
         overwriteFlag, fullInPath, args->ffOptions, fullOutPath);

        int systemCode = system(ffmpegCall);
        
        if (systemCode != EXIT_SUCCESS) {
            printError("call to FFmpeg failed", strerror(errno));
        } else {
            runtimeData->convertedFiles++;
        }

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

        free(outPath);
        free(fullInPath);
        free(fullOutPath);
        free(ffmpegCall);
        free(overwriteFlag);
    }

    closedir(dir);

    return EXIT_SUCCESS;
}

static bool _isDirectory(const char *pathToFile) {
    struct stat pathStats;
    stat(pathToFile, &pathStats);

    return S_ISREG(pathStats.st_mode) == 0 ? true : false;
}