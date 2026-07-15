# 00 READ FIRST — TABLET TRAINING ARTIFACTS MASTER

Read this file before every tablet session. If a rule is unclear, stop and
report the ambiguity. Do not invent an interpretation.

## LEVELS
- Level 0: ASCII codes 32–127 inclusive. Code 32 is SPACE; 33–126 are visible; 127 is DEL and has no visible glyph.
- Level 1: counting 1–1000.
- Level 2: ABCs.

## DIRECTORY CONTENTS — EXACT ALLOWLIST
Only these governance files and signed tablet XML files live directly here:
- `00_READ_FIRST_RULES_AND_LAWS.md`
- `INDEX_MASTER.xml`
- `TEMPLATE_tablet.xml`
- `EXAMPLE_L1_counting_ones_0002_2026-07-15.xml`
- `EXAMPLE_L1_counting_twos_0001_2026-07-15.xml`
- signed tablet XML files following the naming convention below

No data files, scripts, scratch notes, or bulk imports belong here.

## TABLET RULES
1. A tablet is one XML object containing one header and one or more stones.
2. Every tablet uses exactly the element names shown in the template.
3. The header contains title, level, zero or more repeatable prerequisites, submitter, date, and replacement.
4. Every stone contains exactly `visual_art`, `audio_art`, and `text`.
5. A nonempty artifact contains exactly one `file_path`. It is the complete repository-relative filename and location. Do not repeat the same fact in a header pull list.
6. Curriculum order exists only in `INDEX_MASTER.xml`. Tablet headers and filenames do not contain curriculum order.
7. Tablet XML contains no `seconds` or `duration` value.
8. The build derives the run manifest from each stone and assigns the stored-inference span.
9. For initial Level 1 concepts, 1 is held for 1 second, 2 for 2 seconds, through 10 for 10 seconds. Audio plays at its real speed within that span.
10. Timing for 11–1000 requires a separate signed rule before those stored inferences enter.
11. Empty `visual_art` invokes the resident renderer. Counting renders numeral, word, quoted word, and count marks. ABCs render uppercase and lowercase glyphs. Other text renders the word from frozen 8x8 letters.
12. Empty `audio_art` reads `text` from approved resident sound atoms. A nonempty `audio_art` path must exist and carry signal.
13. Data paths are permanent and undated. A correction creates a new data file; it does not move the signed file.
14. An indexed tablet is complete. Pending placeholders never enter the active curriculum.
15. Every index entry includes submitter, date, and the exact replaced filename or `none`.
16. Counting and ABC tablets may omit prerequisites because they establish the first ground. Later grounded tablets must contain at least one `<prereq>` supplied by Tyler or his written list.

## COUNTING VISUAL RULE
For number `n`, render all four together: the numeral (`1`), the word (`one`),
the quoted word (`"one"`), and exactly `n` count marks. Count marks fill
left-to-right in rows of ten. Ten rows form one 10x10 hundred square. Above
100, show completed hundred squares and the active partial square. Compute the
picture from `n`; do not create or store one picture file per number.

Each frozen 8x8 letter fits one `uint64_t`. A 10x10 count square does not; its
positions are computed as `row = cell / 10` and `column = cell % 10`. This same
arrangement is reserved for later multiplication and decimal grounding, whose
specific rules will be signed separately.

## NAMING
- Tablet: `L<level>_<title>_<seq4>_<YYYY-MM-DD>.xml`
- Visual: `Genesis/Eden/Visual_Artifacts/lvl<level>_<series>_<item>.<ext>`
- Sound: `Genesis/Eden/Sound_artifacts/lvl<level>_<series>_<item>.wav`
- Index: `INDEX_MASTER.xml`
- Template: `TEMPLATE_tablet.xml`

Visual and sound files for the same item use the same filename stem. The same
permanent artifact may be referenced by multiple tablets.

## ELEMENT DEFINITIONS
- `tablet`: the complete tablet object.
- `header`: tablet identity, prerequisites, and submission record.
- `title`: lowercase underscore title matching the tablet filename.
- `level`: curriculum level integer.
- `prereq`: one prerequisite title; repeatable and optional in examples.
- `submitted_by`: person or model submitting the tablet.
- `date`: submission date in `YYYY-MM-DD`.
- `replaces`: exact replaced tablet filename or `none`.
- `stone`: one token and its domain.
- `visual_art`: optional additional picture. Empty means use the deterministic renderer for `text` and the stone domain.
- `audio_art`: optional recorded sound. Empty means read `text` from approved resident sound atoms.
- `file_path`: the one complete repository-relative filename and location for its enclosing artifact.
- `text`: exact textual form used by the renderer, reader, and convergence.
