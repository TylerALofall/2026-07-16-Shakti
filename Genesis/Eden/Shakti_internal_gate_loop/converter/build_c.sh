#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
out=${1:-${TMPDIR:-/tmp}/eden-converter-bin}
cc=${CC:-cc}
mkdir -p "$out"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/eden_import_old.c" -o "$out/eden_import_old"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/eden_factor_color.c" -o "$out/eden_factor_color"
printf '%s\n' "$out/eden_import_old" "$out/eden_factor_color"
