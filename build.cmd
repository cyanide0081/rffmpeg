@rem Build script for Windows using Clang (MSVC libs)
@echo off

call :compile %~1
call :clean
exit /b

@rem actual recursive compilation loop
:compile

set        "CC=clang"
set     "FLAGS=-I./lib -I./lib/dirent -std=c11 -Wall -Wextra -pedantic"
set "WIN_FLAGS=-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE -lshell32 -lkernel32 -lmsvcrt -Xlinker /NODEFAULTLIB:libcmt"
set   "D_FLAGS=-g -gcodeview"
set   "R_FLAGS=-DNDEBUG -O2"
set   "P_FLAGS=-finstrument-functions -DINSTRUMENTATION"
set   "BIN_DIR=bin"

setlocal enabledelayedexpansion

set "files="

@rem remember to add (*.res) later too (for icon)
for /R %%f in (*.c) do (
	set "files=!files! %%f"
)

mkdir "%BIN_DIR%" >NUL 2>&1
set "cmd="
echo.

if /I "%~1" == "rel" (
    echo Building rffmpeg in RELEASE mode...
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %R_FLAGS%"
) else if /I "%~1" == "prof" (
    echo Building rffmpeg in PROFILING mode...
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %D_FLAGS% %P_FLAGS%"
) else (
    echo Building rffmpeg in DEBUG mode...
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %D_FLAGS%"
)

@echo on
call %cmd%
@echo off

setlocal disabledelayedexpansion

:clean

@rem del /Q /S *.o *.obj *.ilk *.pdb >NUL 2>&1
