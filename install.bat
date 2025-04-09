:: install.bat
@echo off
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Must be run as administrator
    exit /b 1
)

echo Installing library and headers...

set LIBDEST=C:\Program Files\MyLib\lib
set INCLUDEDEST=C:\Program Files\MyLib\include

mkdir "%LIBDEST%" 2>nul
mkdir "%INCLUDEDEST%" 2>nul

copy /Y libllist.dll "%LIBDEST%"
copy /Y llist.h "%INCLUDEDEST%"

:: Optionally update system PATH
setx PATH "%PATH%;%LIBDEST%" /M

echo Installation complete!
