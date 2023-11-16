@rem Build script for Windows using Clang (MSVC libs)
@echo off

call :compile %~1
exit /b

:compile
set "CC=clang"
set "FLAGS=-I./lib -I./lib/dirent -std=c99 -Wall -Wextra -pedantic"
set "WIN_FLAGS=-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -lshell32 -lkernel32 -lmsvcrt -Xlinker /NODEFAULTLIB:libcmt"
set "D_FLAGS=-g -gcodeview"
set "R_FLAGS=-DNDEBUG -O2"
set "P_FLAGS=-finstrument-functions -DINSTRUMENTATION"
set "BIN_DIR=bin"

setlocal enabledelayedexpansion

@rem TODO: remember to add (*.res) later too (for icon)
for /R %%f in (*.c) do (
	set "files=!files! %%f"
)

mkdir "%BIN_DIR%" > NUL 2>&1

if /I "%~1" == "rel" (
    set "msg=Building rffmpeg in RELEASE mode..."
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %R_FLAGS%"
) else if /I "%~1" == "prof" (
    set "msg=Building rffmpeg in PROFILING mode..."
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %D_FLAGS% %P_FLAGS%"
) else (
    set "msg=Building rffmpeg in DEBUG mode..."
    set "cmd=%CC% -o %BIN_DIR%/rffmpeg.exe %files% %FLAGS% %WIN_FLAGS% %D_FLAGS%"
)

echo [1;44m%msg%[0m
@echo on
call %cmd%
@echo off

if /I "%~1" == "rel" (
    echo:
    echo [1;41mRemoving debug files ^(.pdb, .ilk, ...^)[0m
    call :clean
)

setlocal disabledelayedexpansion
goto :eof

:clean
del /Q /S *.o *.obj *.ilk *.pdb >NUL 2>&1
goto :eof
