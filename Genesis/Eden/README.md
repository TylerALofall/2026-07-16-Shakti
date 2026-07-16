# GENESIS — EDEN / SHAKTI

## DIRECTORY MAP — READ BEFORE DOING ANYTHING
- `Genesis/Eden/README.md` — the door: purpose, locations, naming, first and last actions.
- `Genesis/Eden/LAWS.md` — binding project laws.
- `Genesis/Eden/MODEL_INSTRUCTIONS.md` — current state and work in progress.
- `Genesis/Eden/SIGN_LEDGER.md` — one signed line for every exact path added, modified, or removed.
- `Genesis/Eden/Tablet_Training_Artifacts_Master/` — tablet rules, template, examples, and `INDEX_MASTER.xml`.
- `Genesis/Eden/Visual_Artifacts/` — permanent undated visual data files.
- `Genesis/Eden/Sound_artifacts/` — permanent undated sound data files.
- `Genesis/Eden/Relationship_Masters/` — reviewed themed relationship and drill tables for build-time C conversion.
- `Genesis/Eden/Shakti_internal_gate_loop/` — C17 runtime gate and build-time conversion tools.
- `Genesis/Eden/Shakti_Spirit_instructions/` — approved Markdown instructions.
- `Genesis/Eden/Shakti_Inference/` — stored converged inference records.
- `Genesis/Eden/Shakti_Schools_Scores/` — school results and receipts.

## THREE FILES EVERY MODEL MUST READ
1. `Genesis/Eden/README.md`
2. `Genesis/Eden/LAWS.md`
3. `Genesis/Eden/MODEL_INSTRUCTIONS.md`

Read all three before changing anything.

## PURPOSE
This public repository is the clean house, not a copy of the working forge.
Only correct, complete, reviewable files enter. Do not bulk-import the old tree.

## FIRST AND LAST ACTION
FIRST: read the three required files and
`Genesis/Eden/Tablet_Training_Artifacts_Master/00_READ_FIRST_RULES_AND_LAWS.md`.

LAST: add one exact-path line to `Genesis/Eden/SIGN_LEDGER.md` for every file
added, modified, or removed. Do not use wildcards.

## ARTIFACT NAMING
Visual and sound data use `lvl<level>_<series>_<item>.<ext>` and the same stem
for the same item. Data names carry no dates. A signed data path never moves.

Examples: `lvl1_counting_one.svg`, `lvl1_counting_one.wav`, `lvl2_ABCs_Aa.svg`.

Level 2 supplementary alphabet art follows
`Genesis/Eden/Visual_Artifacts/ALPHABET_ART_SPEC.md`. It uses three complete
512x512 SVG sets named `block`, `italic`, and `pair`. These pictures never
replace the frozen 8x8 `visual_text` identity.

## RELATIONSHIP TABLES AND DRILLS
Relationship rows are small, themed, and source-receipted. They are class rows,
not a growing all-pairs link graph. The build computes lesson views by exact
filtering, classification joins, ordering, and formulas. Candidates and refused
rows never enter Ground. Runtime reads resident const tables, never `.xlsx`.

Exhaustive arithmetic is declared as a finite domain, not thousands of handmade
pictures. For example, nested integer loops cover every accepted `a + b` or
`a - b` row; the C visual renderer computes dots, rows of ten, colors, symbols,
and the exact result. Repeated colored passes belong to the caller's run
manifest and do not duplicate the mathematical fact.

## TABLETS, PREREQUISITES, AND ORDER
Each tablet is one XML object. XML declares only the tablet's direct relevant
prerequisites. It does not schedule training, repetition, or practice. The
calling script reads `INDEX_MASTER.xml`, follows prerequisite titles, refuses
missing prerequisites or cycles, and calls tablets in the resulting order.

Each stone has four fixed load positions, in this order:

1. `visual_text` — generated from the stone by the frozen 8x8 glyph renderer.
2. `visual_art` — optional real picture.
3. `audio_art` — optional recording.
4. `text` — exact symbols or words.

An optional slot stays present as an empty element. A nonempty artifact contains
one `file_path`: the complete repository-relative filename and location. The
fixed C enum is defined once in the tablet rules; no tablet repeats an enum.

Tablet headers use `date_added` and `last_modified`. Correct a tablet in place,
update `last_modified`, update its index entry, and sign the exact path.

## ROOTS AND VISUAL TEXT
Level 0 is fast ASCII 32–127 pre-exposure, not a prerequisite. Level 1 is
counting 1–1000. Level 2 is ABCs. Counting and ABC tablets may have no
prerequisite because they establish the first ground.

For counting, `visual_text` shows numeral, word, quoted word, and exact marks.
Marks fill left-to-right in rows of ten; ten rows make a hundred square. For
ABCs it shows uppercase and lowercase glyphs. Other tablets render their text
as a sequence of frozen `uint64_t` 8x8 glyphs.

## TIMING
Tablets contain no timing fields. The calling script assigns the stored span.
For initial Level 1 concepts, 1 is held for 1 second through 10 for 10 seconds.
Audio plays at its recorded speed inside that span. A signed rule is required
before stored inferences for 11–1000 enter.

## ABSOLUTE
NO PYTHON. Do not generate, edit, test, rename, or move Eden files with Python.
Use inspectable C17 and approved shell build entrypoints.
