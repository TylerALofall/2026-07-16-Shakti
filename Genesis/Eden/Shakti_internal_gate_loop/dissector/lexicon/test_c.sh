#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=${TMPDIR:-/tmp}/eden-lexicon-test-$$
sh "$here/build_c.sh" "$tmp"
"$tmp/test_lexicon"
if "$tmp/eden_matrix_to_c" "$here/tests/unsorted_matrix.txt" > "$tmp/refused.h" 2> "$tmp/refused.err"; then
  echo "FAIL: unsorted matrix was accepted" >&2
  exit 1
fi
grep -q "REFUSED row 2" "$tmp/refused.err"
echo "PASS: unsorted matrix refused"
