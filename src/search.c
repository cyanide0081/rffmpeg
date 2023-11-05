#include <search.h>

extern Arena *globalArena;

static char **_getFilesFromDir(
    const char *dir, const char **fmts, const bool recurse
);

char **getFiles(const Arguments *args) {
    size_t listSize = LIST_BUF;
    char **list = GlobalArenaPush(listSize * sizeof(char*));
    size_t listIdx = 0;

    for (int i = 0; args->inPaths[i]; i++) {
        const char *dir = args->inPaths[i];
        char *trimmedDir = trimUTF8StringTo(dir + PREFIX_LEN, LINE_LEN - 40);

        if (!isDirectory(dir)) {
            printf(
                "%s \"%s\"%s is not a directory (%signoring%s)\n\n",
                COLOR_INPUT, trimmedDir, COLOR_DEFAULT,
                COLOR_ACCENT, COLOR_DEFAULT
            );

            continue;
        }

        printf(
            "%s scanning %s@ %s\"%s\"%s\n\n",
            COLOR_DEFAULT, COLOR_ACCENT, COLOR_INPUT, trimmedDir, COLOR_DEFAULT
        );

        char **files = _getFilesFromDir(
            dir, (const char**)args->inFormats,
            !(args->options & OPT_NORECURSION)
        );

        size_t fileCount = 0;

        if (errno) continue;

        if (files)
            while (files[fileCount])
                fileCount++;

        printf(
            "%s (found %s%zu%s files)%s\n\n",
            COLOR_DEFAULT, COLOR_ACCENT, fileCount, COLOR_DEFAULT, COLOR_DEFAULT
        );

        if (!files) continue;

        for (int idx = 0; files[idx]; idx++) {
            if (listIdx >= listSize - 1) {
                size_t newSize = listSize * 2;

                char **newList = GlobalArenaPush(newSize * sizeof(char*));
                memcpy(newList, list, listSize * sizeof(char*));
                memset(
                    (newList + listSize), 0,
                    (newSize - listSize) * sizeof(char*)
                );

                list = newList;
                listSize = newSize;
            }

            list[listIdx++] = GlobalArenaPushString(files[idx]);
        }
    }

    if (!(*list)) return NULL;

    return list;
}

static char **_getFilesFromDir(
    const char *dir, const char **fmts, const bool recurse
) {
    size_t listSize = LIST_BUF;
    size_t listIdx = 0;
    char **list = GlobalArenaPush(listSize * sizeof(char*));

    errno = 0;

    DIR *d = opendir(dir);
    struct dirent *entry = NULL;

    if (!d) {
        char *err = GlobalArenaSprintf("%s: %s", dir, strerror(errno));
        printErr("unable to open directory", err);

        return NULL;
    }

    while ((entry = readdir(d))) {
#ifdef _WIN32
        int size = UTF16toUTF8(entry->d_name, -1, NULL, 0);
        char *fileName = GlobalArenaPush(size * sizeof(char));
        UTF16toUTF8(entry->d_name, -1, fileName, size);
#else
        char *fileName = GlobalArenaPushString(entry->d_name);
#endif
        char *fullInPath =
            GlobalArenaSprintf("%s%c%s", dir, PATH_SEP, fileName);

        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) continue;

        /* Recursively search in case it's a directory */
        if (isDirectory(fullInPath) && recurse) {
            char **recList = _getFilesFromDir(fullInPath, fmts, recurse);

            if (recList) {
                for (int i = 0; recList[i]; i++) {
                    if (listIdx == listSize - 1) {
                        size_t newSize = listSize * 2;

                        char **newList =
                            GlobalArenaPush(newSize * sizeof(char*));

                        memcpy(newList, list, listSize * sizeof(char*));
                        memset(
                            (newList + listSize), 0,
                            (newSize - listSize) * sizeof(char*)
                        );

                        list = newList;
                        listSize = newSize;
                    }

                    list[listIdx++] = GlobalArenaPushString(recList[i]);
                }
            }

            continue;
        }

        for (int i = 0; fmts[i]; i++) {
            char ext[FILE_BUF];
            snprintf(ext, FILE_BUF, ".%s", fmts[i]);

            const char *extPoint = strstr(fileName, ext);

            if (extPoint && extPoint[strlen(ext)] == '\0') {
                if (listIdx == listSize - 1) {
                    size_t newSize = listSize * 2;

                    char **newList = GlobalArenaPush(newSize * sizeof(char*));
                    memcpy(newList, list, listSize * sizeof(char*));
                    memset(
                        newList + listSize, 0,
                        (newSize - listSize) * sizeof(char*)
                    );

                    list = newList;
                    listSize = newSize;
                }

                list[listIdx++] = GlobalArenaPushString(fullInPath);

                break;
            }
        }
    }

    closedir(d);

    if (!(*list)) return NULL;

    list[listIdx] = NULL;
    return list;
}
