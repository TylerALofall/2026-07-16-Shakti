# GENESIS — EDEN / SHAKTI

## DIRECTORY MAP — READ BEFORE DOING ANYTHING
- `Genesis/Eden/README.md` — the door: purpose, locations, naming, first and last actions.
- `Genesis/Eden/LAWS.md` — binding project laws.
- `Genesis/Eden/MODEL_INSTRUCTIONS.md` — current state and work in progress.
- `Genesis/Eden/SIGN_LEDGER.md` — append-only record of every exact file added, replaced, or removed.
- `Genesis/Eden/Tablet_Training_Artifacts_Master/` — tablet rules, exact template, examples, and `INDEX_MASTER.xml`.
- `Genesis/Eden/Visual_Artifacts/` — permanent undated visual data files.
- `Genesis/Eden/Sound_artifacts/` — permanent undated sound data files.
- `Genesis/Eden/Shakti_internal_gate_loop/` — C source for the resident life loop.
- `Genesis/Eden/Shakti_Spirit_instructions/` — approved Markdown instruction files.
- `Genesis/Eden/Shakti_Inference/` — stored converged inference records.
- `Genesis/Eden/Shakti_Schools_Scores/` — school verdicts and receipts.

## THREE FILES EVERY MODEL MUST READ
1. `Genesis/Eden/README.md`
2. `Genesis/Eden/LAWS.md`
3. `Genesis/Eden/MODEL_INSTRUCTIONS.md`

Read all three before creating, changing, moving, or deleting anything.

## PURPOSE
This public repository is the clean house. It is not a copy of the working
forge. Do not bulk-move old project files here. Only correct, complete,
submittable files enter, one reviewed group at a time.

## FIRST AND LAST ACTION
FIRST: read the three required files above and then read
`Genesis/Eden/Tablet_Training_Artifacts_Master/00_READ_FIRST_RULES_AND_LAWS.md`.

LAST: append one line for every exact file added, replaced, or removed to
`Genesis/Eden/SIGN_LEDGER.md`. Wildcards and directory summaries are refused.

## ARTIFACT NAMING
Visual and sound artifacts use the same filename stem:
`lvl<level>_<series>_<item>.<ext>`.

Examples:
- `lvl1_counting_one.svg`
- `lvl1_counting_one.wav`
- `lvl2_ABCs_Aa.svg`

An artifact may be used by more than one tablet or level. It is referenced in
place; it is never copied merely because it is used again. Data filenames carry
no dates. Data files never move after a signed tablet references them.

## TABLETS AND ORDER
Every tablet is one XML object. It contains its header, repeatable
prerequisites, exact pull filenames and repository-relative paths, and stones
using the approved element names. Curriculum order exists only in
`INDEX_MASTER.xml`; changing one entry's `order` value is the one-line reorder.

## TIMING
Tablet XML never contains a guessed `seconds` or `duration` value. Timing is
assigned when the run manifest creates the stored inference.

For the initial Level 1 number concepts, the stored inference span is exact:
1 is held for 1 second, 2 for 2 seconds, through 10 for 10 seconds. Audio plays
at its real playback speed inside that span. A signed rule must define later
counting timing before stored inferences for 11–1000 are admitted.

## ABSOLUTE
NO PYTHON. Do not generate, edit, test, rename, or move Eden files with Python.
Use inspectable C and the approved shell build entry only.

