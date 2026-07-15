@echo off
setlocal
set "HERE=%~dp0"
if "%CC%"=="" set "CC=clang"
if "%~1"=="" (set "OUT=%TEMP%\eden-converter-bin") else (set "OUT=%~1")
if not exist "%OUT%" mkdir "%OUT%"
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%eden_import_old.c" -o "%OUT%\eden_import_old.exe" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%eden_factor_color.c" -o "%OUT%\eden_factor_color.exe" || exit /b 1
echo %OUT%\eden_import_old.exe
echo %OUT%\eden_factor_color.exe
