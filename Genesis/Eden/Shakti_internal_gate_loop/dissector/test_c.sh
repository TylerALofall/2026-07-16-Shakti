#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=${TMPDIR:-/tmp}/eden-dissector-test-$$
sh "$here/build_c.sh" "$tmp"
"$tmp/test_number_lexer"
"$tmp/eden_number_demo" "twenty-one plus four"
