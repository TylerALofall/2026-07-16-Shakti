# 00 READ FIRST — TABLET TRAINING ARTIFACTS MASTER
**Established 2026-07-15 by Tyler (T-Lo), Founder. READ THIS FILE BEFORE EVERY
SESSION and before creating ANY tablet or artifact. Nothing here is ambiguous;
if something seems ambiguous, it is a defect — report it, do not interpret it.**

---

## 1. THE LEVELS (exact, corrected 2026-07-15)
- **LEVEL 0** = ASCII characters **32–127** (starts with SPACE, 0x20). The glyphs
  are run through FAST, FIRST, so nothing is ever brand-new mid-sentence.
  Controlled novelty: she never meets a defining-factor surprise mid-stream.
- **LEVEL 1** = COUNTING, 1–1000.
- **LEVEL 2** = ABCs (the alphabet, Aa Bb case pairs).
- Higher levels follow; every tablet declares its level and its prerequisites.

## 2. THE LAWS OF THIS DIRECTORY
1. **ONE OBJECT.** A tablet is one XML object containing: header (title, level,
   prerequisites, order, signing) + the pull list (every file it needs, with
   paths) + its stones. Nothing a tablet needs lives outside its pull list.
2. **REORDERABLE IN ONE LINE.** Curriculum order lives in `<order>` in the header
   and in one line of INDEX_MASTER.xml. Reordering = editing one line. Never
   renumber files to reorder.
3. **DATA FILES NEVER MOVE AND CARRY NO DATES.** Sounds, pictures, renders keep
   one permanent undated path forever (e.g. `atoms/one.wav`,
   `renders/one.svg`). Only XML/CML documents go directly in THIS directory,
   and only XML carries dates.
4. **CONSISTENT ELEMENT NAMES ACROSS ALL STONES.** The schema in §4 is the only
   schema. No invented tags, ever. A stone that needs a new element is a
   proposal to Tyler, not an improvisation.
5. **SLOTTED = UNTOUCHABLE.** Once a tablet is signed into INDEX_MASTER.xml it
   is never edited. A correction is a NEW dated file whose index entry names
   what it `replaces`. No correction churn.
6. **SILENT OBJECTS READ THE WORD.** If a stone has no recorded sound, its
   sound fallback is reading the word (letter atoms). Silence is neutral —
   see the Non-Contradiction Law below.
7. **PICTURE DEFAULT = THE WORD RENDERED**, 8×8 per letter (font8x8
   public-domain baseline). The model may WRITE HER OWN LETTERS on her canvas
   — doing is understanding — but the floor's 36 reference glyphs are frozen.
8. **TIMING LAW.** The recorded sound's own duration IS the time unit. "one"
   costs ~1s; "one hundred" costs its true length. Inference timing follows
   recorded truth. At convergence, the three senses are pulled SIMULTANEOUSLY
   from the index via a RUN MANIFEST — three readers, one moment.
9. **NON-CONTRADICTION CONVERGENCE LAW (2026-07-15).** Each sense gives one of
   three verdicts on the focal claim: SUPPORTS / SILENT / CONTRADICTS. Silence
   is neutral, never negative (the quiet bird in the grass still passes).
   Contradiction must be POSITIVE and must cite a row in a closed exclusivity
   table (one color per surface; one place at a time; loud-means-near). Absence
   is never in the table. PASS = at least one sense supports AND zero
   contradict. UNKNOWN = all silent. FLAG = a named exclusivity row fired.
   Focus vs field: each moment has ONE focus; field items (the humming AC) are
   SILENT about the focus, never a mismatch.
10. **HEARING IS TWO INPUTS** (like two eyes): two channels at slightly
   different times carry direction and depth. Mono atoms are lawful today;
   the schema reserves the second channel.
11. **PREREQUISITES** are written by Tyler (or given by him to the creator).
   They go in the header, with level and title, and MAY BE REPEATED.
12. **THE INDEX IS SIGNED.** Every INDEX_MASTER.xml entry carries: submitted_by,
   date, and replaces (the exact filename it replaces, or "none"). No unsigned
   entries. No undated entries.

## 3. NAMING CONVENTION (exact)
- **Tablet XML:** `L<level>_<title>_<seq4>_<YYYY-MM-DD>.xml`
  Example: `L1_counting_ones_0001_2026-07-15.xml`
  - level: integer, matches header. title: lowercase, underscores, no spaces.
  - seq4: 4-digit sequence within that title. date: submission date.
- **The index (exact name, never varies):** `INDEX_MASTER.xml` — this is the
  table-of-contents file. It lives in this directory. It is read before any
  tablet is loaded. It is the ONLY place curriculum order lives.
- **This rules file (exact name):** `00_READ_FIRST_RULES_AND_LAWS.md`.
- **Data files (undated, permanent):** `atoms/<word>.wav` (sound),
  `renders/<word>.svg` (pictures), `tone_atoms/<char>.wav` (key tones).
  These paths NEVER change once a tablet references them.

## 4. THE HEADER SCHEMA — every element defined, no exceptions
```
<tablet>
  <header>
    <title>       lowercase_underscore title, matches filename        </title>
    <level>       integer: 0,1,2,...                                   </level>
    <order>       4-digit curriculum position; the ONE reorder line     </order>
    <prereq>      a prerequisite tablet TITLE; repeatable, may repeat   </prereq>
    <submitted_by> who made this file (Tyler / model name)             </submitted_by>
    <date>        YYYY-MM-DD of submission                             </date>
    <replaces>    exact filename replaced, or the word none            </replaces>
    <pull>        the RUN MANIFEST: every file this tablet needs
      <file role="sound"  path="atoms/one.wav"/>      role: sound|visual|tone
      <file role="visual" path="renders/one.svg"/>    path: permanent, undated
    </pull>
  </header>
  <stone token="..." domain="...">   token: the exact word/symbol; domain: its class-group
    <visual seconds="1.0">           seconds: display span (timing law: truth over targets)
      <render ref="renders/one.svg"/>  ref: permanent path from the pull list, or empty
      <examples>text field examples</examples>
    </visual>
    <sound seconds="1.0" channels="1">  channels: 1 today; 2 reserved (binaural law)
      <say>one</say>                    say: the word to read (silent-object fallback)
      <record ref="atoms/one.wav"/>     ref: permanent path, must pass wav_has_signal
    </sound>
    <text>
      <word tier="2">one</word>
      <letters tier="1">o n e</letters>
      <context>one seen in: ...</context>
    </text>
    <bond>   how this stone re-sees known stones; computed joins only, NO manual links </bond>
    <converge token="..."/>   the focal claim; verdicts per Non-Contradiction Law
  </stone>
</tablet>
```
Element names above are THE names. `visual` `sound` `text` `bond` `converge`
`say` `record` `render` `word` `letters` `context` — never synonyms, never new.

## 5. WHAT LIVES IN THIS DIRECTORY
- `00_READ_FIRST_RULES_AND_LAWS.md` (this file)
- `INDEX_MASTER.xml` (the signed table of contents — exact name)
- `TEMPLATE_tablet.xml` (copy to start any tablet)
- `EXAMPLE_L1_counting_ones_0001_2026-07-15.xml`
- `EXAMPLE_L2_alphabet_a_0001_2026-07-15.xml`
- Signed tablet XML files, per the naming convention. NOTHING ELSE. No data
  files, no scripts, no notes.
