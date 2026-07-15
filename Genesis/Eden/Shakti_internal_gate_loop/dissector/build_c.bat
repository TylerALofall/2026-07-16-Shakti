@echo off
setlocal
set "HERE=%~dp0"
if "%CC%"=="" set "CC=clang"
if "%~1"=="" (set "OUT=%TEMP%\eden-dissector-bin") else (set "OUT=%~1")
if not exist "%OUT%" mkdir "%OUT%"
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic -c "%HERE%eden_number_lexer.c" -o "%OUT%\eden_number_lexer.o" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%eden_number_demo.c" "%OUT%\eden_number_lexer.o" -o "%OUT%\eden_number_demo.exe" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%test_number_lexer.c" "%OUT%\eden_number_lexer.o" -o "%OUT%\test_number_lexer.exe" || exit /b 1
echo %OUT%\eden_number_demo.exe
echo %OUT%\test_number_lexer.exe
