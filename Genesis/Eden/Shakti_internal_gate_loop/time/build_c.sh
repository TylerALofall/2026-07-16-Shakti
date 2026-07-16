#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
out=${1:-${TMPDIR:-/tmp}/eden-clock-bin}
cc=${CC:-cc}
mkdir -p "$out"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic -c "$here/eden_clock.c" -o "$out/eden_clock.o"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/eden_clock_flow.c" "$out/eden_clock.o" -o "$out/eden_clock_flow"
"$cc" -std=c17 -O2 -Wall -Wextra -Werror -pedantic "$here/test_clock.c" "$out/eden_clock.o" -o "$out/test_clock"
printf '%s\n' "$out/eden_clock_flow" "$out/test_clock"
