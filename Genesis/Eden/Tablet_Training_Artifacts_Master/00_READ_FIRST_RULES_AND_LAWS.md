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
3. The header contains title, level, zero or more repeatable prerequisites, submitter, date, replacement, and a complete pull list.
4. Every pull entry contains both the exact filename and its repository-relative path.
5. Curriculum order exists only in `INDEX_MASTER.xml`. Tablet headers and filenames do not contain curriculum order.
6. Tablet XML contains no `seconds` or `duration` value.
7. The run manifest selects the resident files and assigns the stored-inference span.
8. For initial Level 1 concepts, 1 is held for 1 second, 2 for 2 seconds, through 10 for 10 seconds. Audio plays at its real speed within that span.
9. Timing for 11–1000 requires a separate signed rule before those stored inferences enter.
10. A silent object uses approved sound artifacts to read the word. Empty active sound references are refused.
11. The default picture is the word rendered from approved frozen 8x8 letters.
12. Data paths are permanent and undated. A correction creates a new data file; it does not move the signed file.
13. An indexed tablet is complete. Pending placeholders never enter the active curriculum.
14. Every index entry includes submitter, date, and the exact replaced filename or `none`.
15. Prerequisites are supplied by Tyler or by the creator acting from Tyler's written list. The `<prereq>` element is repeatable.

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
- `header`: tablet identity, prerequisites, submission record, and pulls.
- `title`: lowercase underscore title matching the tablet filename.
- `level`: curriculum level integer.
- `prereq`: one prerequisite title; repeatable and optional in examples.
- `submitted_by`: person or model submitting the tablet.
- `date`: submission date in `YYYY-MM-DD`.
- `replaces`: exact replaced tablet filename or `none`.
- `pull`: complete list of required data files.
- `file`: one required artifact with `role`, `name`, and repository-relative `path`.
- `stone`: one token and its domain.
- `visual`: visual portion of the stone.
- `render`: exact visual path already present in the pull list.
- `examples`: literal examples shown with the stone.
- `sound`: sound portion of the stone; timing is not stored here.
- `say`: text to read when the object is spoken.
- `record`: exact sound path already present in the pull list.
- `text`: textual portion of the stone.
- `word`: whole word or symbol.
- `letters`: exact character sequence.
- `context`: literal teaching context.
- `bond`: the existing Eden bond field; this cleanup does not change its grammar.
- `converge`: names the focal token presented by the run manifest.
