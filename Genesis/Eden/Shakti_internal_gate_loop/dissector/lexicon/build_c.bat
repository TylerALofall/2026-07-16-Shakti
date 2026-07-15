@echo off
setlocal
set "HERE=%~dp0"
if "%CC%"=="" set "CC=clang"
if "%~1"=="" (set "OUT=%TEMP%\eden-lexicon-bin") else (set "OUT=%~1")
if not exist "%OUT%" mkdir "%OUT%"
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic "%HERE%eden_matrix_to_c.c" "%HERE%eden_lexicon.c" -o "%OUT%\eden_matrix_to_c.exe" || exit /b 1
"%OUT%\eden_matrix_to_c.exe" "%HERE%tests\sample_matrix.txt" > "%OUT%\eden_lexicon_generated.h" || exit /b 1
%CC% -std=c17 -O2 -Wall -Wextra -Werror -pedantic -I"%HERE%" -I"%OUT%" "%HERE%test_lexicon.c" "%HERE%eden_lexicon.c" -o "%OUT%\test_lexicon.exe" || exit /b 1
echo %OUT%\eden_matrix_to_c.exe
echo %OUT%\test_lexicon.exe
