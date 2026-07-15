# Exact number-first dissector

This malloc-free C17 component extracts exact integers before full language
dissection. It uses sorted string tables and binary search; it has no hashes,
embeddings, or probabilities.

Routes are deliberately separate:

- `MATH_CANDIDATE`: at least one exact number plus a math operator, relation,
  operation word, or `how many` / `how much`.
- `NUMBER_CONTEXT`: a number exists without enough evidence to call it math.
- `LANGUAGE`: no number was found.
- `REFUSED`: fixed limits, integer overflow, or table integrity failed.

The word parser composes 0–99 from 28 closed entries: zero through nineteen and
the tens. It recognizes forms such as `twenty one` and `twenty-one` without a
100-row handwritten table. Plain digit integers use checked 64-bit parsing.
Larger number words and decimals are held as unresolved instead of being split
into false smaller numbers.

The math route is a candidate route, not an answer. The later deterministic
grammar must still bind quantities, units, the question, and an exact rule.

Build on macOS/Linux with `sh build_c.sh`. Build on Windows with Clang using
`build_c.bat`. Both default to a temporary output directory.
