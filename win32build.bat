@echo off
cd /D "%~dp0"
cls
echo *********************************************************************
echo *                                                                   *
echo * Raptor build script for Windows                                   *
echo *                                                                   *
echo *********************************************************************
echo.
echo Select build:
echo.
echo   1. Release build win32
echo   2. Release build win64
echo   3. Clean all
echo   0. Exit
echo.

set /P select=Select 0-3: 

if /i "%select%"== "1" goto:win32
if /i "%select%"== "2" goto:win64
if /i "%select%"== "3" goto:clean
if /i "%select%"== "0" goto:eof
echo Incorrect entry
goto:eof

:win32:
set arch="Release|x86"
set archname=win32
set buildfoldername=raptorx86
set msvcfolder=msvc\Release
set sdlfolder=include\SDL2-devel-2.28.2-VC\SDL2-2.28.2\lib\x86\SDL2.dll
goto:buildres

:win64:
set arch="Release|x64"
set archname=win64
set buildfoldername=raptorx64
set msvcfolder=msvc\x64\Release
set sdlfolder=include\SDL2-devel-2.28.2-VC\SDL2-2.28.2\lib\x64\SDL2.dll
goto:buildres

:buildres
set digitpos0=false
set digitpos2=false
set digitpos4=false
set version=empty
echo Enter version number in this format 0.0.0
set /P version=Enter version number: 
if %version% == "empty" goto:buildres
if "%version:~0,-4%" geq "0" if "%version:~0,-4%" leq "9" set digitpos0=true
if "%version:~2,-2%" geq "0" if "%version:~2,-2%" leq "9" set digitpos2=true
if "%version:~4%" geq "0" if "%version:~4%" leq "9" set digitpos4=true
if not "%digitpos0%" == "true" (
  echo Incorrect entry
  goto:buildres
)
if not "%digitpos2%" == "true" (
  echo Incorrect entry
  goto:buildres
)
if not "%digitpos4%" == "true" (
  echo Incorrect entry
  goto:buildres
)
if not "%version:~1,-3%" == "." (
  echo Incorrect entry
  goto:buildres
)
if not "%version:~3,-1%" == "." (
  echo Incorrect entry
  goto:buildres
)
(
echo #define APSTUDIO_READONLY_SYMBOLS
echo #include "winres.h"
echo #undef APSTUDIO_READONLY_SYMBOLS
echo.
echo MAINICON ICON "raptor.ico"
echo.
echo VS_VERSION_INFO VERSIONINFO
echo     FILEVERSION %version:~0,-4%,%version:~2,-2%,%version:~4%,0
echo     PRODUCTVERSION %version:~0,-4%,%version:~2,-2%,%version:~4%,0
echo     FILEFLAGSMASK 0x3FL
echo     FILEFLAGS 0x0L
echo     FILEOS 0x4L
echo     FILETYPE 0x1L
echo     FILESUBTYPE 0x0L
echo BEGIN
echo     BLOCK "StringFileInfo"
echo     BEGIN
echo         BLOCK "040904E4"
echo         BEGIN
echo             VALUE "CompanyName", "skynettx"
echo             VALUE "FileDescription", "Raptor %version%"
echo             VALUE "FileVersion", "%version%"
echo             VALUE "InternalName", "raptor"
echo             VALUE "LegalCopyright", "Copyright (C) 1994-%date:~-4%. Licensed under the GNU General Public License, version 2"
echo             VALUE "OriginalFilename", "raptor"
echo             VALUE "ProductName", "Raptor"
echo             VALUE "ProductVersion", "%version%"
echo         END
echo     END
echo     BLOCK "VarFileInfo"
echo     BEGIN
echo         VALUE "Translation", 0x409, 1252
echo     END
echo END
) > rsrc\resource.rc
(
echo #define APSTUDIO_READONLY_SYMBOLS
echo #include "winres.h"
echo #undef APSTUDIO_READONLY_SYMBOLS
echo.
echo MAINICON ICON "raptorsetup.ico"
echo.
echo VS_VERSION_INFO VERSIONINFO
echo     FILEVERSION %version:~0,-4%,%version:~2,-2%,%version:~4%,0
echo     PRODUCTVERSION %version:~0,-4%,%version:~2,-2%,%version:~4%,0
echo     FILEFLAGSMASK 0x3FL
echo     FILEFLAGS 0x0L
echo     FILEOS 0x4L
echo     FILETYPE 0x1L
echo     FILESUBTYPE 0x0L
echo BEGIN
echo     BLOCK "StringFileInfo"
echo     BEGIN
echo         BLOCK "040904E4"
echo         BEGIN
echo             VALUE "CompanyName", "skynettx"
echo             VALUE "FileDescription", "Raptor %version% Setup"
echo             VALUE "FileVersion", "%version%"
echo             VALUE "InternalName", "raptorsetup"
echo             VALUE "LegalCopyright", "GNU General Public License"
echo             VALUE "OriginalFilename", "raptorsetup"
echo             VALUE "ProductName", "Raptor Setup"
echo             VALUE "ProductVersion", "%version%"
echo         END
echo     END
echo     BLOCK "VarFileInfo"
echo     BEGIN
echo         VALUE "Translation", 0x409, 1252
echo     END
echo END
) > rsrc\setup\resource.rc

:build
devenv msvc\raptor.sln /Build %arch%
goto:buildfolder

:buildfolder
@RD /S /Q pkg\win32\%buildfoldername%
mkdir pkg\win32\%buildfoldername%
xcopy %msvcfolder%\raptor.exe pkg\win32\%buildfoldername%
xcopy %msvcfolder%\raptorsetup\raptorsetup.exe pkg\win32\%buildfoldername%
xcopy include\TinySoundFont\LICENSE pkg\win32\%buildfoldername%
ren pkg\win32\%buildfoldername%\LICENSE LICENSETSF
xcopy LICENSE pkg\win32\%buildfoldername%
xcopy include\TimGM6mb\TimGM6mb.sf2 pkg\win32\%buildfoldername%
xcopy %sdlfolder% pkg\win32\%buildfoldername%

if exist pkg\win32\%buildfoldername%\raptor.exe (
  echo raptor.exe PASS
) else (
  echo raptor.exe FAILED
  goto:eof
)
if exist pkg\win32\%buildfoldername%\raptorsetup.exe (
  echo raptorsetup.exe PASS
) else (
  echo raptorsetup.exe FAILED
  goto:eof
)
if exist pkg\win32\%buildfoldername%\LICENSETSF (
  echo LICENSETSF PASS
) else (
  echo LICENSETSF FAILED
  goto:eof
)
if exist pkg\win32\%buildfoldername%\LICENSE (
  echo LICENSE PASS
) else (
  echo LICENSE FAILED
  goto:eof
)
if exist pkg\win32\%buildfoldername%\TimGM6mb.sf2 (
  echo TimGM6mb.sf2 PASS
) else (
  echo TimGM6mb.sf2 FAILED
  goto:eof
)
if exist pkg\win32\%buildfoldername%\SDL2.dll (
  echo SDL2.dll PASS
) else (
  echo SDL2.dll FAILED
  goto:eof
)
goto:getinfo

:getinfo
set assetsflag=false
set /P assets=Bundle assets y/n: 
if "%assets%" == "y" (
  set assetsflag=true
) 
if "%assets%" == "Y" (
  set assetsflag=true
) 
if "%assets%" == "yes" (
  set assetsflag=true
) 
if "%assets%" == "YES" (
  set assetsflag=true
) 
if "%assetsflag%" == "true" (
  goto:getassets
) else (
  echo Continue without bundle assets
  goto:buildnsis
)

:getassets
set /P "assetspath=Path to assets folder: "
xcopy "%assetspath%\*.GLB" pkg\win32\%buildfoldername%

:buildnsis
(
  echo  ; Script generated by the HM NIS Edit Script Wizard.
  echo.
  echo  ; HM NIS Edit Wizard helper defines
  echo  !define PRODUCT_NAME "Raptor"
  echo  !define PRODUCT_VERSION "%version%"
  echo  !define PRODUCT_PUBLISHER "skynettx"
  echo  !define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\raptor.exe"
  echo  !define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  echo  !define PRODUCT_UNINST_ROOT_KEY "HKLM"
  echo.
  echo  ; MUI 1.67 compatible ------
  echo  !include "MUI.nsh"
  echo.
  echo  ; MUI Settings
  echo  !define MUI_ABORTWARNING
  echo  !define MUI_ICON "..\..\rsrc\raptor.ico"
  echo  !define MUI_UNICON "..\..\rsrc\raptor.ico"
  echo.
  echo  ; Welcome page
  echo  !insertmacro MUI_PAGE_WELCOME
  echo  ; Directory page
  echo  !insertmacro MUI_PAGE_DIRECTORY
  echo  ; Instfiles page
  echo  !insertmacro MUI_PAGE_INSTFILES
  echo  ; Finish page
  echo  !define MUI_FINISHPAGE_RUN "$INSTDIR\raptor.exe"
  echo  !insertmacro MUI_PAGE_FINISH
  echo.
  echo  ; Uninstaller pages
  echo  !insertmacro MUI_UNPAGE_INSTFILES
  echo.
  echo  ; Language files
  echo  !insertmacro MUI_LANGUAGE "English"
  echo.
  echo  ; MUI end ------
  echo.
  echo  Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  echo  OutFile "raptor-%version%-%archname%.exe"
  if "%archname%" == "win32" (
    echo  InstallDir "$PROGRAMFILES\Raptor"
  ) else (
    echo  InstallDir "$PROGRAMFILES64\Raptor"
  )
  echo  InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
  echo  ShowInstDetails show
  echo  ShowUnInstDetails show
  echo.
  echo Section "Hauptgruppe" SEC01
  echo   SetOutPath "$INSTDIR"
  echo   SetOverwrite try
  if "%assetsflag%" == "true" (
    echo   File "%buildfoldername%\FILE0000.GLB"
    echo   File "%buildfoldername%\FILE0001.GLB"
  )
  echo   File "%buildfoldername%\LICENSE"
  echo   File "%buildfoldername%\LICENSETSF"
  echo   File "%buildfoldername%\raptor.exe"
  echo   CreateDirectory "$SMPROGRAMS\Raptor"
  echo   CreateShortCut "$SMPROGRAMS\Raptor\Raptor.lnk" "$INSTDIR\raptor.exe"
  echo   CreateShortCut "$DESKTOP\Raptor.lnk" "$INSTDIR\raptor.exe"
  echo   File "%buildfoldername%\raptorsetup.exe"
  echo   CreateShortCut "$SMPROGRAMS\Raptor\Raptor Setup.lnk" "$INSTDIR\raptorsetup.exe"
  echo   CreateShortCut "$DESKTOP\Raptor Setup.lnk" "$INSTDIR\raptorsetup.exe"
  echo   File "%buildfoldername%\SDL2.dll"
  echo   File "%buildfoldername%\TimGM6mb.sf2"
  echo SectionEnd
  echo.
  echo Section -AdditionalIcons
  echo   CreateShortCut "$SMPROGRAMS\Raptor\Uninstall.lnk" "$INSTDIR\uninst.exe"
  echo SectionEnd
  echo.
  echo Section -Post
  echo   WriteUninstaller "$INSTDIR\uninst.exe"
  echo   WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\raptor.exe"
  echo   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  echo   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  echo   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\raptor.exe"
  echo   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  echo   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  echo SectionEnd
  echo.
  echo.
  echo Function un.onUninstSuccess
  echo   HideWindow
  echo   MessageBox MB_ICONINFORMATION^|MB_OK "Raptor wurde erfolgreich deinstalliert."
  echo FunctionEnd
  echo.
  echo Function un.onInit
  echo   MessageBox MB_ICONQUESTION^|MB_YESNO^|MB_DEFBUTTON2 "Möchten Sie Raptor und alle seinen Komponenten deinstallieren?" IDYES +2
  echo   Abort
  echo FunctionEnd
  echo.
  echo Section Uninstall
  echo   Delete "$INSTDIR\uninst.exe"
  echo   Delete "$INSTDIR\TimGM6mb.sf2"
  echo   Delete "$INSTDIR\SDL2.dll"
  echo   Delete "$INSTDIR\raptorsetup.exe"
  echo   Delete "$INSTDIR\raptor.exe"
  echo   Delete "$INSTDIR\LICENSETSF"
  echo   Delete "$INSTDIR\LICENSE"
  if "%assetsflag%" == "true" (
    echo   Delete "$INSTDIR\FILE0001.GLB"
    echo   Delete "$INSTDIR\FILE0000.GLB"
  )
  echo.
  echo   Delete "$SMPROGRAMS\Raptor\Uninstall.lnk"
  echo   Delete "$DESKTOP\Raptor Setup.lnk"
  echo   Delete "$SMPROGRAMS\Raptor\Raptor Setup.lnk"
  echo   Delete "$DESKTOP\Raptor.lnk"
  echo   Delete "$SMPROGRAMS\Raptor\Raptor.lnk"
  echo.
  echo   RMDir "$SMPROGRAMS\Raptor"
  echo   RMDir "$INSTDIR"
  echo.
  echo   DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  echo   DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  echo   SetAutoClose true
  echo SectionEnd
) > pkg\win32\build.nsi
makensis.exe pkg\win32\build.nsi
xcopy pkg\win32\raptor-%version%-%archname%.exe %cd%
del pkg\win32\raptor-%version%-%archname%.exe
goto:eof

:clean:
@RD /S /Q msvc\.vs
@RD /S /Q msvc\Release
@RD /S /Q msvc\x64
@RD /S /Q pkg\win32
del *.exe
echo All cleaned
goto:eof

:eof