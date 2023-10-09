#ifndef H_LIBS
#define H_LIBS

/* CRT headers */
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
#include <inttypes.h>
#include <stdarg.h>

/* OS headers */
#if defined _WIN32
#include <windows.h>
#include <shellapi.h>
#include <fcntl.h>
#include <io.h>
#elif defined __linux__ || defined __APPLE__ || defined BSD
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/cdefs.h>
#include <sys/mman.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif /* __linux__ */
#else
#error "unsupported OS/kernel"
#endif /* OS headers */
#endif /* H_LIBS */
