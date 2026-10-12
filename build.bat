@echo off
cd /d "%~dp0"
where gcc >nul 2>nul
if errorlevel 1 set "PATH=C:\msys64\ucrt64\bin;%PATH%"
gcc -std=c11 -Wall -Wextra -Werror main.c integer.c fraction.c input.c output.c cli.c -o converter.exe
if errorlevel 1 exit /b 1
echo Build successful: converter.exe
