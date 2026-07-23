#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
TMP="$ROOT/tests/tmp_mvp"

rm -rf "$TMP"
mkdir -p "$TMP"
cp -R "$ROOT/data" "$TMP/data"
cp -R "$ROOT/eden_out" "$TMP/eden_out"

(
    cd "$TMP"
    "$ROOT/shakti" --check > check_output.txt
    "$ROOT/shakti" --demo > demo_output.txt
)

grep -q 'Runnable beginning: YES.' "$TMP/check_output.txt"
grep -q 'PASS 1' "$TMP/demo_output.txt"
grep -q 'Demo complete. School mastery remains unchanged.' \
    "$TMP/demo_output.txt"

rm -rf "$TMP"
mkdir -p "$TMP"
cp -R "$ROOT/data" "$TMP/data"
cp -R "$ROOT/eden_out" "$TMP/eden_out"

cat > "$TMP/input.txt" <<'INPUT'
/shakti_run/ pass 4
/shakti_run/ school +
+
+
+
+
+
+
+
+
+
+
/quit/
INPUT

(
    cd "$TMP"
    "$ROOT/shakti" < input.txt > drill_output.txt
)

grep -q '10/10 — 100% MASTERY' "$TMP/drill_output.txt"
grep -q 'History preserved: 10 correct, 0 errors, 10 attempts.' \
    "$TMP/drill_output.txt"

RIGHT_COUNT=$(grep -c '|RIGHT|+|4|' "$TMP/data/school/school_state.log")
test "$RIGHT_COUNT" -eq 10

echo "Runnable startup, Pass 1 demo, and ten-streak School drill passed."
