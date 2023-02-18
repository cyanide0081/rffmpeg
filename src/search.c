#include "../include/search.h"


bool isDirectory(const char *pathToFile);

int searchDirectory(const char *directory, arguments *args, processInfo *runtimeData) {
    const char *inputPath = directory == NULL ? args->inPaths[0] : directory;
    size_t inFormatIndex = 0;

    static char *newFolderName = NULL;

    if (newFolderName == NULL)
        newFolderName = args->options & OPT_CUSTOMFOLDERNAME ?
         args->customFolderName : args->outFormat;
    

    DIR *dir = opendir(inputPath);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        fprintf(stderr, "%sERROR: %sCouldn't open \'%s\' (%s%s%s)\n\n",
         CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath, CHARCOLOR_RED, strerror(errno), CHARCOLOR_WHITE);

        return ERROR_FAILED_TO_OPEN_DIRECTORY;
    }

    while ((entry = readdir(dir)) != NULL) {
        const char *fileName = entry->d_name;
        const char *inputFormat = NULL;

        /* Skip . and .. */
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)
            continue;

        /* Avoid recursing into the brand new folder */
        if (strcmp(fileName, newFolderName) == 0 && args->options & OPT_NEWFOLDER)
            continue;

        size_t fullInPathSize = snprintf(NULL, 0, "%s/%s", inputPath, fileName) + 1;

        char *fullInPath = xcalloc(fullInPathSize, sizeof(char));
        sprintf(fullInPath, "%s/%s", inputPath, fileName);

        /* Perform recursive search (or not) */
        if (isDirectory(fullInPath) && !(args->options & OPT_NORECURSION)) {
            searchDirectory(fullInPath, args, runtimeData);

            free(fullInPath);
            continue;
        }

        bool isOfFormat = false;

        for (int i = 0; args->inFormats[i]; i++) {
            if (strstr(fileName, args->inFormats[i]) != NULL) {
                isOfFormat = true;
                inFormatIndex = i;
                break;
            }
        }

        if (!isOfFormat) {
            free(fullInPath);
            continue;
        }

        /* Copy filename except the extension */
        char pureFileName[FILE_BUFFER];
        strncpy(pureFileName, fileName,
         (strlen(fileName) - strlen(args->inFormats[inFormatIndex]) - 1));

        char *outPath = NULL;

        /* Make-a-subfolder-or-not part */
        /* TODO: implement the newpath alternative */
        if (args->options & OPT_NEWFOLDER) {
            size_t newPathSize = snprintf(NULL, 0, "%s/%s", inputPath, newFolderName) + 1;
            char *newPath = xcalloc(newPathSize, sizeof(char));

            sprintf(newPath, "%s/%s", inputPath, newFolderName);

            FILE *f = fopen(newPath, "w"); // Create directory

            outPath = newPath;
        } else {
            outPath = strdup(inputPath);
        }

        char *overwriteFlag = args->options & OPT_OVERWRITE ? strdup("-y") : strdup("");

        if (!(args->options & OPT_OVERWRITE))
            preventFilenameOverwrites(pureFileName, args->outFormat, outPath);

        size_t fullOutPathSize = snprintf(NULL, 0,
         "%s/%s.%s", outPath, pureFileName, args->outFormat) + 1;

        char *fullOutPath = xcalloc(fullOutPathSize, sizeof(char));//uuu

        sprintf(fullOutPath, "%s/%s.%s", outPath, pureFileName, args->outFormat);

        size_t ffmpegCallSize = snprintf(NULL, 0, "ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
         overwriteFlag, fullInPath, args->ffOptions, fullOutPath) + 1;

        char *ffmpegCall = xcalloc(ffmpegCallSize, sizeof(char));
        sprintf(ffmpegCall, "ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
         overwriteFlag, fullInPath, args->ffOptions, fullOutPath);

        int systemCode = system(ffmpegCall);
        
        if (systemCode != EXIT_SUCCESS) {
            fprintf(stderr, "%sERROR:%s call to FFmpeg failed (%s%s%s)\n\n",
             CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_RED, strerror(errno), CHARCOLOR_WHITE);
        } else {
            runtimeData->convertedFiles++;
        }

        printf("\n");
        
        /* Keep or delete original files */
        if (args->options & OPT_CLEANUP) {
            if (remove(fullInPath) != 0) {
                fprintf(stderr, "%sERROR:%s couldn't delete original file (%s%s%s)\n\n",
             CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_RED, strerror(errno), CHARCOLOR_WHITE);
            } else {
                runtimeData->deletedFiles++;
            }
        }

        /* Delete new folder in case it exists and no conversions succeeded */
        if (args->options & OPT_NEWFOLDER && runtimeData->convertedFiles == 0)
            remove(outPath);

        free(fullInPath);
        free(overwriteFlag);
        free(outPath);
        free(fullOutPath);
        free(ffmpegCall);
    }

    closedir(dir);

    return EXIT_SUCCESS;
}

bool isDirectory(const char *pathToFile) {
    struct stat pathStats;
    stat(pathToFile, &pathStats);

    return S_ISREG(pathStats.st_mode) == 0 ? true : false;
}