# RESIDENT RELATIONSHIP TABLE CONTRACT

This directory defines the fixed C row layout for relationship masters. The
future build-time converter reads reviewed TSV, refuses unsorted or non-admitted
rows, interns exact strings into one const blob, and emits const arrays.

Runtime rules:

- C17, fixed storage, no heap, subprocess, COM, or daemon;
- exact strings and binary search, never hash identity;
- no stored all-pairs edges;
- graphs are temporary computed views over theme, relation, class, order, and
  formula columns;
- only `ADMITTED` input is emitted;
- `COUNTED` and `FOUNDER_DEFINED` rows remain marked and cannot become
  `ABSOLUTE` by joining them.

`eden_relationship_schema.h` is the layout contract, not a populated table.
