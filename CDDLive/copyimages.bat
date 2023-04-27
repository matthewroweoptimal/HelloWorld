@echo off


if "%1"=="help" goto HELP
if "%1"=="HELP" goto HELP
if "%1"=="/?" goto HELP

if "%1"=="" set IMAGES_DIR=.\IMAGES
if "%1"=="" set DEBUG_DIR=.\IMAGES\Debug
if not "%1"=="" set IMAGES_DIR=.\IMAGES\%1
if not "%1"=="" set DEBUG_DIR=.\IMAGES\%1\Debug

echo ============================================================
echo Running copyimages to populate %IMAGES_DIR% directory
echo ============================================================

if not exist %IMAGES_DIR% md %IMAGES_DIR%
del %IMAGES_DIR%\* /F /Q
xcopy "Release\OLY_APPLICATION*.img"       %IMAGES_DIR% /y /i
xcopy "Release 8\OLY_APPLICATION*.img"     %IMAGES_DIR% /y /i
xcopy "Release 12\OLY_APPLICATION*.img"    %IMAGES_DIR% /y /i
xcopy "Release 15\OLY_APPLICATION*.img"    %IMAGES_DIR% /y /i
xcopy "Release 118\OLY_APPLICATION*.img"   %IMAGES_DIR% /y /i
xcopy "Release 218\OLY_APPLICATION*.img"   %IMAGES_DIR% /y /i
xcopy "Release Manf\OLY_APPLICATION*.img"  %IMAGES_DIR% /y /i

xcopy "..\BOOTLOADER\BOOTLOADER.bin"  %IMAGES_DIR% /y /i

REM Copying of Debug Images commented out for now. 
REM if not exist %DEBUG_DIR% md %DEBUG_DIR%
REM del %DEBUG_DIR%\* /F /Q
REM xcopy "Debug\OLY_APPLICATION*.img"       %DEBUG_DIR% /y /i
REM xcopy "Debug 8\OLY_APPLICATION*.img"     %DEBUG_DIR% /y /i
REM xcopy "Debug 12\OLY_APPLICATION*.img"    %DEBUG_DIR% /y /i
REM xcopy "Debug 15\OLY_APPLICATION*.img"    %DEBUG_DIR% /y /i
REM xcopy "Debug 118\OLY_APPLICATION*.img"   %DEBUG_DIR% /y /i
REM xcopy "Debug 218\OLY_APPLICATION*.img"   %DEBUG_DIR% /y /i
REM xcopy "Debug Manf\OLY_APPLICATION*.img"  %DEBUG_DIR% /y /i


echo Done, %IMAGES_DIR% directory now populated.
goto END

:HELP
echo Usage of this batch file :
echo ==========================
echo  To copy the CDDLive image files into the IMAGES directory :
echo.
echo        copyimages subdir
echo.
echo        subdir   - Optional sub-directory under the IMAGES folder.
echo.
echo        e.g.   copyimages V1.1.16
echo.


:END
