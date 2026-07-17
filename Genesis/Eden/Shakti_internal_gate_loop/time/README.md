# EDEN CLOCK FLOW

This C17 module proves the small clock model before any old visual assets enter
the clean repository. It has no heap allocation, runtime file loading, daemon,
COM component, vectors, embeddings, or probabilistic selection.

## THE 132-POSITION LAW

The semantic clock floor has 60 second positions, 60 minute positions, and 12
hour positions: 132 positions total. Arithmetic enumerates every state in one
twelve-hour cycle:

```text
cycle_second   = total_seconds % 43200
second_position = cycle_second % 60
minute_position = (cycle_second / 60) % 60
hour_position   = (cycle_second / 3600) % 12
```

Nothing stores 43,200 links. The same 132 classified positions serve all 43,200
states. `hour_subposition` is also derived at minute resolution so a later
analog renderer may move the short hand between hour labels; its 720 values are
computed, never stored as links.

## TWO TRAINING LAPS

`eden_clock_flow` resets to 12:00:00 for each phase.

1. `SECONDS_REAL` walks 60 second states. Its default caller interval is 1000
   ms, so the lap takes one real minute.
2. `MINUTES_VISUAL` walks 60 minute states. Its default caller interval is 100
   ms, so one hour is visibly compressed into six seconds.

Each frame prints the exact time and three one-hot position rows. The XML never
contains these intervals. The caller may choose other explicit intervals; this
changes presentation speed, never the time facts or state formula.

## BUILD AND RUN

POSIX shell, including macOS:

```text
sh build_c.sh
/tmp/eden-clock-bin/test_clock
/tmp/eden-clock-bin/eden_clock_flow --animate
```

Windows with Clang:

```text
build_c.bat
%TEMP%\eden-clock-bin\test_clock.exe
%TEMP%\eden-clock-bin\eden_clock_flow.exe --animate
```

Useful receipt mode:

```text
sh test_c.sh
```

`--no-wait` emits every state without delay. `--seconds-only` and
`--minutes-only` isolate one lap. `--seconds-ms N` and `--minutes-ms N` accept
an explicit 0–60000 millisecond caller interval. `--animate` uses the standard
ANSI clear-screen sequence; without it, frames remain plain append-only text.

## SOURCE AND ADMISSION BOUNDARY

The exact minute, hour, and day conversions are receipted in
`Relationship_Masters/time_conversions.tsv` from the BIPM SI Brochure, 9th
edition, version 4.01, section 4, Table 8.

This module does not admit a clock tablet or copy the old `clocks/` assets. The
old private source folder must first be readable, then each asset must be
reviewed against the four-slot tablet contract. No guessed or bulk-moved file
belongs in the clean house.
