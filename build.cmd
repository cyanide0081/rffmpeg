@rem Build script for Windows using MSVC compiler
@echo off

call :compile %~1
call :clean
exit /b

@rem actual recursive compilation loop
:compile

set   "FLAGS=/I .\lib\dirent shell32.lib /std:c17 /W4 /wd4996 /wd4100"
set "D_FLAGS=/Zi /MDd"
set "R_FLAGS=/Os /favor:blend /MD"
set "BIN_DIR=.\bin\"

setlocal enabledelayedexpansion

set "files="

for /R %%f in (*.c) do (
	set "files=!files! %%f"
)

mkdir "%BIN_DIR%"

if /I "%~1" == "rel" (
	cl.exe /Fe"%BIN_DIR%\rffmpeg.exe" %files% %FLAGS% %R_FLAGS%
) else (
	cl.exe /Fe"%BIN_DIR%\rffmpeg.exe" %files% %FLAGS% %D_FLAGS%
)

setlocal disabledelayedexpansion

:clean

del /Q /S *.obj *.ilk *.pdb >NUL 2>&1
