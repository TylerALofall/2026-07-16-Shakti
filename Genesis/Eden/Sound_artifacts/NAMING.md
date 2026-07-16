# Sound artifacts

Name source atoms `lvl<level>_<series>_<item>.wav`.

Use 16 kHz mono 16-bit PCM WAV. Every referenced file must pass the signal
check. The deterministic audio composer refuses 48 kHz/16 kHz mixtures and does
not resample. Earlier 48 kHz drafts are not inputs to the 16 kHz output set.

Derived math audio uses the exact stem of its matching SVG:

- `math_Add_A-C.wav`
- `math_Sub_A-C.wav`
- `math_Mul_A-C.wav`
- `math_Div_A-C.wav`

Question and answer tracks live in separate generated directories, each with a
`MATH_AUDIO_MANIFEST.tsv` receipt. The manifest records the exact ordered atoms,
sample count, and 16,000 Hz rate. A generated set is not complete when any
required founder-voice atom is missing.

Files carry no date and never move after admission. Letter recordings enter only
after verified cuts exist. Tablet XML contains no timing fields. NO PYTHON.
