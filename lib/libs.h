#ifndef H_LIBS
#define H_LIBS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#ifndef NDEBUG
#define dprintf(fmt, ...) fprintf(stderr, "%s:%d:%s(): " fmt, \
                                  __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define dprintf(...)
#endif

/* OS-dependent headers */
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <fcntl.h>
#include <io.h>
#elif defined __unix__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#endif

#endif // H_LIBS
