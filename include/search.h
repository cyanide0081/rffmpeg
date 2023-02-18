#ifndef H_MAINLOOP
#define H_MAINLOOP

#include "libs.h"
#include "types.h"
#include "parsers.h"
#include "handlers.h"
#include "terminal.h"

int searchDirectory(const char *directory, arguments *args, processInfo *runtimeData);

#endif // H_MAINLOOP i