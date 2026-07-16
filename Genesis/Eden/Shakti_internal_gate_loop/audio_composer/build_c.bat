@echo off
setlocal

where clang >nul 2>nul
if %errorlevel%==0 (
    clang -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror eden_math_audio.c -o eden_math_audio.exe
    if errorlevel 1 exit /b 1
    echo PASS built eden_math_audio.exe with clang
    exit /b 0
)

where cl >nul 2>nul
if %errorlevel%==0 (
    cl /nologo /std:c17 /O2 /W4 /WX eden_math_audio.c /Fe:eden_math_audio.exe
    if errorlevel 1 exit /b 1
    echo PASS built eden_math_audio.exe with MSVC
    exit /b 0
)

echo REFUSED no C17 compiler found. Install LLVM clang or Visual Studio C tools.
exit /b 2
