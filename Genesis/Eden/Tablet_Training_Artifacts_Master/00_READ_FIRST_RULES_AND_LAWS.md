# 00 READ FIRST — TABLET TRAINING ARTIFACTS MASTER

Read this before every tablet session. Do not invent missing rules.

## LEVELS
- Level 0: fast ASCII 32–127 pre-exposure. 32 is SPACE; 33–126 are visible; 127 is DEL.
- Level 1: counting 1–1000.
- Level 2: ABCs.

## EXACT DIRECTORY ALLOWLIST
- `00_READ_FIRST_RULES_AND_LAWS.md`
- `INDEX_MASTER.xml`
- `TEMPLATE_tablet.xml`
- `EXAMPLE_L1_counting_ones_0002_2026-07-15.xml`
- `EXAMPLE_L1_counting_twos_0001_2026-07-15.xml`
- complete signed tablet XML files matching the naming rule

No data files, scripts, scratch notes, or bulk imports belong here.

## TABLET RULES
1. One tablet is one XML object with one header and one or more stones.
2. Use only the element names and order shown in the template.
3. Header fields are `title`, `level`, zero or more `prereq` elements, `submitted_by`, `date_added`, and `last_modified`.
4. Every stone contains exactly `visual_text`, `visual_art`, `audio_art`, and `text`, in that order.
5. `visual_text` is generated and has no `file_path`.
6. Empty optional artifacts are written `<visual_art/>` or `<audio_art/>` so their load positions remain fixed.
7. A nonempty artifact contains exactly one `file_path`: one complete repository-relative filename and location.
8. Do not add a per-tablet enum, pull list, separate name/path, timing field, or training-control field.
9. Tablet XML declares direct prerequisites; it does not schedule training, repetition, or practice.
10. The calling script reads `INDEX_MASTER.xml`, follows prerequisites, and refuses a missing title or cycle.
11. Curriculum order exists only in `INDEX_MASTER.xml`; one `order` edit reorders a tablet.
12. Counting and ABC tablets may omit `prereq`. Other tablets list only direct relevant parents; repeat `prereq` for multiple parents.
13. A tablet edit keeps the same filename, updates `last_modified`, updates the index entry, and is signed in the ledger.
14. Data paths are permanent and undated. Corrected data receives a new stable path.
15. An indexed tablet is complete. Pending placeholders never enter the curriculum.
16. A referenced WAV must exist and carry signal.
17. Tablets contain no timing fields. The caller assigns the stored span.
18. Initial counting 1–10 uses spans of 1–10 seconds. Rules for 11–1000 must be signed before use.

## FIXED LOAD ENUM
Define this once in C, never in individual XML files:

```c
enum EdenLoadSlot {
    EDEN_LOAD_VISUAL_TEXT = 0,
    EDEN_LOAD_VISUAL_ART  = 1,
    EDEN_LOAD_AUDIO_ART   = 2,
    EDEN_LOAD_TEXT        = 3,
    EDEN_LOAD_SLOT_COUNT  = 4
};
```

The four positions always exist. An empty optional artifact means "no file for
this slot," not a shifted index.

## VISUAL TEXT
`visual_text` is the deterministic text picture. Each character is one frozen
8x8 `uint64_t`; a word is an ordered glyph sequence. It never depends on the
host's installed fonts.

For counting domain token `n`, show numeral, word, quoted word, and exactly `n`
marks. Marks fill left-to-right in rows of ten. Ten rows make a hundred square.
Above 100, reuse completed squares plus the active square. Compute positions as
`row = cell / 10` and `column = cell % 10`.

For ABC domain, show uppercase and lowercase glyphs. `visual_art` remains free
for an optional real picture in every domain.

## NAMING
- Tablet: `L<level>_<title>_<seq4>_<YYYY-MM-DD>.xml`
- Visual data: `Genesis/Eden/Visual_Artifacts/lvl<level>_<series>_<item>.<ext>`
- Sound data: `Genesis/Eden/Sound_artifacts/lvl<level>_<series>_<item>.wav`
- Index: `INDEX_MASTER.xml`
- Template: `TEMPLATE_tablet.xml`

## ELEMENT DEFINITIONS
- `tablet`: complete tablet object.
- `header`: identity, prerequisites, and submission record.
- `title`: lowercase underscore title matching the tablet filename.
- `level`: curriculum level integer.
- `prereq`: one direct relevant prerequisite title; repeatable.
- `submitted_by`: original submitter.
- `date_added`: first admission date in `YYYY-MM-DD`.
- `last_modified`: latest signed edit date in `YYYY-MM-DD`.
- `stone`: one token and domain.
- `visual_text`: generated frozen-glyph channel; always present and empty in XML.
- `visual_art`: optional real picture; empty or one `file_path`.
- `audio_art`: optional recording; empty or one `file_path`.
- `file_path`: complete repository-relative filename and location.
- `text`: exact textual form used in convergence.
