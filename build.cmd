@rem Build script for Windows using MSVC compiler
@echo off

call :compile %~1
call :clean
exit /b

@rem actual recursive compilation loop
:compile

set   "FLAGS=/I .\include\dirent shell32.lib /std:c17 /W4 /wd4996 /wd4100"
set "D_FLAGS=/Zi /MDd"
set "R_FLAGS=/Os /favor:blend /MD"
set     "BIN=.\build\rffmpeg.exe"

setlocal enabledelayedexpansion

set "files="

for /R %%f in (*.c) do (
	set "files=!files! %%f"
)

mkdir ".\build"

if /I "%~1" == "rel" (
	cl.exe /Fe%BIN% %files% %FLAGS% %R_FLAGS%
) else (
	cl.exe /Fe%BIN% %files% %FLAGS% %D_FLAGS%
)

setlocal disabledelayedexpansion

:clean

del /Q /S *.obj *.ilk *.pdb >NUL 2>&1
