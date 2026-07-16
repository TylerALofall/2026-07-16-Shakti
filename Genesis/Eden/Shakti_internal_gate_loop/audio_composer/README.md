# Deterministic Math Audio Composer

This C17 build tool joins Tyler's already-clipped founder-voice WAV atoms into
audio tracks whose stems exactly match the 1,600 question SVGs and 1,600 answer
SVGs:

- `math_Add_A-C.wav`
- `math_Sub_A-C.wav`
- `math_Mul_A-C.wav`
- `math_Div_A-C.wav`

It uses no Python, malloc, hashes, vectors, probabilities, COM, daemons, network,
or runtime loader. It never resamples and never overwrites an existing output.
Every atom must be PCM, mono, 16-bit, 16,000 Hz and must contain a sample whose
absolute value is greater than 200.

## Exact speech rules

- Question: `A` + operator + `C` + `equals`
- Answer: `A` + operator + `C` + `equals` + answer
- `200` is `two hundred`.
- `201` is `two hundred and one`.
- `325` is `three hundred and twenty five`.
- A negative answer begins with the existing `minus.wav` atom.
- No silent gap is invented by default. An exact gap of 0..16,000 zero-valued
  samples may be requested explicitly.

Operator mapping is closed and literal: Add=`plus.wav`, Sub=`minus.wav`,
Mul=`times.wav`, Div=`divide.wav`, followed by `equals.wav`.

## Required atom paths

The atom root must contain the paths used by the attached 69-file archive:

```text
counting/lvl1_counting_one.wav ... lvl1_counting_ninety.wav
counting/lvl1_counting_hundred.wav
counting/lvl1_counting_and.wav
UNFILED_no_level_yet/plus.wav
UNFILED_no_level_yet/minus.wav
UNFILED_no_level_yet/times.wav
UNFILED_no_level_yet/divide.wav
UNFILED_no_level_yet/equals.wav
```

The clean repository's flat shelf layout is also accepted: for example,
`lvl1_counting_one.wav` and `plus.wav` directly inside the atom root. Resolution
is exact and closed. If both the nested and flat path exist for one word, the
tool refuses the ambiguity.

Complete answer generation also requires Tyler's real recording at:

```text
counting/lvl1_counting_zero.wav
```

The equivalent flat path is `lvl1_counting_zero.wav`.

Without it, `answers` and `all` refuse before writing any output. There are 20
subtraction cards whose answer is zero. The 69-file archive does not contain that
atom; the tool does not synthesize or substitute Tyler's voice.

## Build on macOS or Linux

```sh
chmod +x build_c.sh
./build_c.sh
```

`CC=clang ./build_c.sh` selects clang explicitly.

## Build on Windows

Open a compiler command prompt and run:

```bat
build_c.bat
```

The batch file uses clang when available and otherwise MSVC `cl`.

## Verify before writing

```sh
./eden_math_audio preflight "/path/to/sound artifacts ALL69 16k" questions
./eden_math_audio preflight "/path/to/sound artifacts ALL69 16k" all
```

The first command passes with the attached archive. The second correctly refuses
until `lvl1_counting_zero.wav` is added.

## Build matching audio

All 1,600 question tracks can be built now:

```sh
./eden_math_audio build "/path/to/atoms" "/path/to/math_audio" questions
```

After the real zero atom is added, build all 3,200 tracks:

```sh
./eden_math_audio build "/path/to/atoms" "/path/to/math_audio" all
```

Outputs are placed in `math_audio/questions/` and `math_audio/answers/`. Each
directory also receives `MATH_AUDIO_MANIFEST.tsv` with exact operands, answer,
sample count, sample rate, and the ordered atom trace actually copied into each
WAV. For example, `323` is receipted as
`counting:three|counting:hundred|counting:and|counting:twenty|counting:three`.
Use a new or empty output directory for each run.

To insert a deliberate 50 ms gap at 16 kHz, pass exactly `800` as the last
argument. Omitting it means zero inserted samples.

## One-file checks

```sh
./eden_math_audio single "/path/to/atoms" out.wav answer Add 11 10
./eden_math_audio single "/path/to/atoms" out.wav answer Mul 20 20
./eden_math_audio single "/path/to/atoms" out.wav answer Sub 1 20
```

## Color plus balls

The same fixed engine can join the clipped colors and `balls.wav`:

```sh
./eden_math_audio color-balls "/path/to/atoms" blue_balls.wav blue
./eden_math_audio color-balls "/path/to/atoms" two_blue_balls.wav blue 2
```

The closed colors in the current archive are blue, brown, green, orange, purple,
red, and yellow. Counts may be 0..400; count zero requires the real zero atom.
