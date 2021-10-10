@echo off

make

set folder=..\..\Bataille_Navale_Release
set mainDir="%cd%"

if not exist "%folder%" mkdir "%folder%"

cd /d "%folder%"
for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)

if not exist "bin_x86" mkdir "bin_x86"
if not exist "bin_x64" mkdir "bin_x64"
if not exist "fonts" mkdir "fonts"
if not exist "images" mkdir "images"
if not exist "sound" mkdir "sound"

cd %mainDir%

xcopy ".\x32\*.*" "%folder%\bin_x86" /E
copy ".\BT_32.exe" "%folder%\bin_x86"
xcopy ".\x64\*.*" "%folder%\bin_x64" /E
copy ".\BT_64.exe" "%folder%\bin_x64"
copy "..\initSettings.ini" "%folder%\initSettings.ini"
xcopy "..\sound" "%folder%\sound" /E
xcopy "..\images" "%folder%\images" /E
xcopy "..\fonts" "%folder%\fonts" /E

copy "..\config_Default.cfg" "%folder%\game.cfg"
copy "..\clientInfo.info" "%folder%"

echo CLIENT > "%folder%\client.info"

echo 0 > "%folder%\initSettings.ini"

pause

@echo on