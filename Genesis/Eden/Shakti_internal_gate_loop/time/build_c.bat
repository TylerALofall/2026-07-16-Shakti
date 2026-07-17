@echo off
setlocal
set "HERE=%~dp0"
if "%CC%"=="" set "CC=clang"
if "%~1"=="" (set "OUT=%TEMP%\eden-clock-bin") else (set "OUT=%~1")
if not exist "%OUT%" mkdir "%OUT%"
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic -c "%HERE%eden_clock.c" -o "%OUT%\eden_clock.o" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%eden_clock_flow.c" "%OUT%\eden_clock.o" -o "%OUT%\eden_clock_flow.exe" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%test_clock.c" "%OUT%\eden_clock.o" -o "%OUT%\test_clock.exe" || exit /b 1
echo %OUT%\eden_clock_flow.exe
echo %OUT%\test_clock.exe
