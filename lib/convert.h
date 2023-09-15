#ifndef HEADER_CONVERT
#define HEADER_CONVERT

#include <libs.h>
#include <types.h>
#include <parsers.h>
#include <handlers.h>

int convertFiles(const char **files,
                 arguments *args,
                 processInfo *stats);

#endif  /* HEADER_CONVERT */
