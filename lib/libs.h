#ifndef H_LIBS
#define H_LIBS

/* CRT headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sys/cdefs.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

/* OS headers */
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <fcntl.h>
#include <io.h>
#elif defined __unix__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif /* __linux__ */
#endif /* OS headers */
#endif /* H_LIBS */
