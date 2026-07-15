# MODEL INSTRUCTIONS — CURRENT STATE

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

## CURRENT PUBLIC-REPOSITORY STATE
This repository is the clean destination, not the working forge. Do not import
the older project tree. Existing permanent data stays in its current visual and
sound directories. Governance and tablet-contract corrections are handled
before additional curriculum files are admitted.

The resident floor, dictionary matrix, question dissector, inference work, and
life-loop work remain in the forge until each graduating file is complete,
receipted, and signed for this repository.

## CURRENT WORK
1. Correct and freeze the tablet rules, template, examples, and index.
2. Build the C XML-to-const-C validator/converter.
3. Admit Level 0 ASCII tablets only after their required artifacts exist.
4. Admit Level 1 counting tablets in signed order.
5. Define and sign timing for 11–1000 before those stored inferences enter.
6. Admit Level 2 ABC tablets only after their required artifacts contain signal.

## TABLET CONTRACT
- One tablet is one XML object.
- The tablet header contains title, level, optional repeatable prerequisites, submitter, date, and replacement.
- Every stone contains exactly `visual_art`, `audio_art`, and `text`.
- A nonempty artifact contains one `file_path`, which is the complete repository-relative filename and location.
- Empty `visual_art` deterministically renders `text`; empty `audio_art` reads `text` from approved resident atoms.
- Counting and ABC tablets may omit prerequisites because they establish the first ground. Later grounded tablets must name signed prerequisites.
- Tablet headers contain no curriculum order and no duration.
- `INDEX_MASTER.xml` is the sole curriculum order.
- The build derives the run manifest from each stone and assigns stored-inference timing.

## COUNTING VISUAL
The counting renderer shows numeral, word, quoted word, and exact count marks.
Marks fill rows of ten; ten rows make one hundred square. Values above 100 reuse
completed squares plus the active square. Do not create a stored picture for
every number. `uint64_t` stores each 8x8 letter; the 10x10 count arrangement is
computed from the number.

## BEFORE WORK
Read the three mandatory files and
`Genesis/Eden/Tablet_Training_Artifacts_Master/00_READ_FIRST_RULES_AND_LAWS.md`.

## AFTER WORK
Append one line per exact added, replaced, or removed path to
`Genesis/Eden/SIGN_LEDGER.md`. Do not use wildcards or grouped directory names.
