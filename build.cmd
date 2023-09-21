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
set   "D_FLAGS=-ggdb"
set   "R_FLAGS=-DNDEBUG -Os"
set   "BIN_DIR=bin"

setlocal enabledelayedexpansion

set "files="

@rem remember to add (*.res) later too
for /R %%f in (*.c) do (
	set "files=!files! %%f"
)

mkdir "%BIN_DIR%" >NUL 2>&1

if /I "%~1" == "rel" (
	%CC% -o "%BIN_DIR%/rffmpeg.exe" %files% %FLAGS% %WIN_FLAGS% %R_FLAGS%
) else (
	%CC% -o "%BIN_DIR%/rffmpeg.exe" %files% %FLAGS% %WIN_FLAGS% %D_FLAGS%
)

setlocal disabledelayedexpansion

:clean

del /Q /S *.o *.obj *.ilk *.pdb >NUL 2>&1
