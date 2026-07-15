#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
out=${1:-${TMPDIR:-/tmp}/eden-lexicon-bin}
cc=${CC:-cc}
mkdir -p "$out"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic \
  "$here/eden_matrix_to_c.c" "$here/eden_lexicon.c" -o "$out/eden_matrix_to_c"
"$out/eden_matrix_to_c" "$here/tests/sample_matrix.txt" > "$out/eden_lexicon_generated.h"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic -I"$here" -I"$out" \
  "$here/test_lexicon.c" "$here/eden_lexicon.c" -o "$out/test_lexicon"
printf '%s\n' "$out/eden_matrix_to_c" "$out/test_lexicon"
