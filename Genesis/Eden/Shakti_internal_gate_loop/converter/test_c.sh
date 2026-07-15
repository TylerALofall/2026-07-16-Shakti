#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=${TMPDIR:-/tmp}/eden-converter-test-$$
bin=$tmp/bin
mkdir -p "$tmp"
sh "$here/build_c.sh" "$bin"
"$bin/eden_import_old" "$here/tests/valid_counting.xml" "$tmp/counting.tbl"
cmp "$here/tests/expected_counting.tbl" "$tmp/counting.tbl"
if "$bin/eden_import_old" "$here/tests/malformed.xml" "$tmp/refused.tbl"; then
  echo "FAIL: malformed XML was accepted" >&2
  exit 1
fi
"$bin/eden_factor_color" "$here/tests/valid_pairs.tbl" "$tmp/colors.tbl" "$tmp/objects.tbl" "$tmp/factor.receipt"
cmp "$here/tests/expected_colors.tbl" "$tmp/colors.tbl"
cmp "$here/tests/expected_objects.tbl" "$tmp/objects.tbl"
if "$bin/eden_factor_color" "$here/tests/duplicate_pairs.tbl" "$tmp/no-colors.tbl" "$tmp/no-objects.tbl" "$tmp/no.receipt"; then
  echo "FAIL: duplicate pair was accepted" >&2
  exit 1
fi
if "$bin/eden_factor_color" "$here/tests/missing_pair.tbl" "$tmp/no2-colors.tbl" "$tmp/no2-objects.tbl" "$tmp/no2.receipt"; then
  echo "FAIL: incomplete product was accepted" >&2
  exit 1
fi
printf 'PASS: converter acceptance and refusal tests\n'
