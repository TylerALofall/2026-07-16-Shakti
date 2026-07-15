# MODEL INSTRUCTIONS — CURRENT STATE

## DIRECTORY MAP — READ BEFORE DOING ANYTHING
- `Genesis/Eden/README.md` — the door: purpose, locations, naming, first and last actions.
- `Genesis/Eden/LAWS.md` — binding project laws.
- `Genesis/Eden/MODEL_INSTRUCTIONS.md` — current state and work in progress.
- `Genesis/Eden/SIGN_LEDGER.md` — one signed line for every exact path added, modified, or removed.
- `Genesis/Eden/Tablet_Training_Artifacts_Master/` — tablet rules, template, examples, and `INDEX_MASTER.xml`.
- `Genesis/Eden/Visual_Artifacts/` — permanent undated visual data files.
- `Genesis/Eden/Sound_artifacts/` — permanent undated sound data files.
- `Genesis/Eden/Shakti_internal_gate_loop/` — C17 runtime gate and build-time conversion tools.
- `Genesis/Eden/Shakti_Spirit_instructions/` — approved Markdown instructions.
- `Genesis/Eden/Shakti_Inference/` — stored converged inference records.
- `Genesis/Eden/Shakti_Schools_Scores/` — school results and receipts.

## THREE FILES EVERY MODEL MUST READ
1. `Genesis/Eden/README.md`
2. `Genesis/Eden/LAWS.md`
3. `Genesis/Eden/MODEL_INSTRUCTIONS.md`

Read all three before changing anything.

## CURRENT STATE
This repository is the clean destination, not the working forge. Existing data
stays in place. The resident floor, dictionary, inference engine, and gate loop
remain outside this clean tree until each submitted file is complete, tested,
receipted, and signed. The first admitted dissector component is the C17 exact
number-first router in `Shakti_internal_gate_loop/dissector/`.

The old SVG shell generator used a host font instead of the frozen 8x8 glyph
table and is removed. Do not replace it with a placeholder. Submit a complete
cross-platform C17 `visual_text` renderer only after its glyph source and exact
output are verifiable.

## CURRENT WORK
1. Freeze and validate the four-slot tablet schema.
2. Extend the admitted C17 number-first route into deterministic quantity, unit,
   and question binding.
3. Convert the built `form|class|infl|base` matrix to sealed const C and return
   every exact class for ambiguous forms.
4. Audit the existing Sudoku vocabulary routes and symbol sources before
   admitting either; counted vocabulary evidence never becomes absolute math.
5. Make the caller enforce index order, direct prerequisites, and cycle refusal.
6. Add Level 0 ASCII pre-exposure only after its renderer exists.
7. Admit Level 1 counting tablets in signed order.
8. Sign the rule for 11–1000 spans before those stored inferences enter.
9. Admit Level 2 ABC tablets after their referenced audio carries signal.

## DISSECTION ORDER
1. Preserve and tokenize the input.
2. Extract checked digit integers and composed number words 0–99.
3. Route exact-number plus math evidence to `MATH_CANDIDATE`.
4. Route numbers without math evidence to `NUMBER_CONTEXT`.
5. Route input without numbers to `LANGUAGE`.
6. Hold unsupported numeric forms as unresolved; never split or guess them.
7. On a math candidate, bind quantities, units, question, and rule before calling
   absolute arithmetic. The early route never answers by itself.

## TABLET CONTRACT
- One tablet is one XML object.
- Header fields are `title`, `level`, repeatable direct `prereq`, `submitted_by`, `date_added`, and `last_modified`.
- Every stone contains `visual_text`, `visual_art`, `audio_art`, and `text`, in that order.
- `visual_text` is generated; `visual_art` and `audio_art` are optional and remain present when empty.
- A nonempty artifact contains one complete repository-relative `file_path`.
- XML does not control training, repetition, or practice. The calling script does.
- `INDEX_MASTER.xml` is the only curriculum order.
- Tablets contain no timing fields.

## COUNTING VISUAL
Render numeral, word, quoted word, and exact marks. Fill marks in rows of ten;
ten rows make one hundred square. Above 100, reuse completed squares plus the
active square. Each letter is one frozen `uint64_t` glyph; the count grid is
computed arithmetically.

## BEFORE AND AFTER WORK
Before: read the three mandatory files and the tablet rules.

After: append one line per exact added, modified, or removed path to
`Genesis/Eden/SIGN_LEDGER.md`. Do not use wildcards.
