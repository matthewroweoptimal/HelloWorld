@echo off


if "%1"=="help" goto HELP
if "%1"=="HELP" goto HELP
if "%1"=="/?" goto HELP

if "%1"=="" set IMAGES_DIR=.\IMAGES
if not "%1"=="" set IMAGES_DIR=.\IMAGES\%1

if not exist %IMAGES_DIR% md %IMAGES_DIR%

echo ============================================================
echo Running copyimages to populate %IMAGES_DIR% directory
echo ============================================================

xcopy "Release\OLY_APPLICATION*.img"       %IMAGES_DIR% /y /i
xcopy "Release 8\OLY_APPLICATION*.img"     %IMAGES_DIR% /y /i
xcopy "Release 12\OLY_APPLICATION*.img"    %IMAGES_DIR% /y /i
xcopy "Release 15\OLY_APPLICATION*.img"    %IMAGES_DIR% /y /i
xcopy "Release 118\OLY_APPLICATION*.img"   %IMAGES_DIR% /y /i
xcopy "Release 218\OLY_APPLICATION*.img"   %IMAGES_DIR% /y /i
xcopy "Release Manf\OLY_APPLICATION*.img"  %IMAGES_DIR% /y /i

xcopy "..\BOOTLOADER\BOOTLOADER.bin"  %IMAGES_DIR% /y /i


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
