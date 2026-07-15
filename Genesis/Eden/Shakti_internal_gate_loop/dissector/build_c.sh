#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
out=${1:-${TMPDIR:-/tmp}/eden-dissector-bin}
cc=${CC:-cc}
mkdir -p "$out"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic -c "$here/eden_number_lexer.c" -o "$out/eden_number_lexer.o"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/eden_number_demo.c" "$out/eden_number_lexer.o" -o "$out/eden_number_demo"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/test_number_lexer.c" "$out/eden_number_lexer.o" -o "$out/test_number_lexer"
printf '%s\n' "$out/eden_number_demo" "$out/test_number_lexer"
