#!/bin/sh
set -eu

CC_BIN=${CC:-cc}
"$CC_BIN" -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror \
    eden_math_audio.c -o eden_math_audio
printf '%s\n' "PASS built ./eden_math_audio with $CC_BIN"
