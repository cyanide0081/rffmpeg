@rem Build script for Windows using clang (gcc prob works without change)
@echo off

call :compile %~1
call :clean
exit /b

@rem actual recursive compilation loop
:compile

set        "CC=clang"
set     "FLAGS=-I./lib -I./lib/dirent -std=gnu11 -Wall -Wextra -pedantic -lshell32"
set "WIN_FLAGS=-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DUNICODE -D_UNICODE"
set   "D_FLAGS=-ggdb"

@rem TODO: figure out how to link to C:\Windows\System32\msvcrt.dll and reduce exe size
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
