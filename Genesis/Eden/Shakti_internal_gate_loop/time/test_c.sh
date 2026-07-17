#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=${TMPDIR:-/tmp}/eden-clock-test-$$
sh "$here/build_c.sh" "$tmp"
"$tmp/test_clock"
"$tmp/eden_clock_flow" --no-wait --seconds-only >/dev/null
printf '%s\n' "PASS sixty-frame visual flow"
