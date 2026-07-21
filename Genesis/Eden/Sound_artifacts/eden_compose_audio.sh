#!/bin/sh
# eden_compose_audio.sh -- compose the spoken number from REAL recorded atoms.
# the ~30 atoms are recorded ONCE in a real voice; this stitches any 0..1000.
# NO synthesized audio. it emits the plan + the real stitch command (sox handles
# WAV headers). the atoms are yours to record; this never fakes a voice.
#
#   plan     <n> -> the atoms + stitch command for one number
#   timeline <n> -> lay the atoms on the TIME AXIS. real durations read from the
#                   wav headers when recorded; honestly empty until then. sound is
#                   amplitude ACROSS TIME -- time is the spine it is written on, never
#                   a separate neighbor. same law for video: frames across time.
#                   inseparable. sound without time is a ball of useless data.
#   atoms        -> the exact unique atoms to record (the whole vocabulary)
# tools: sh, sed, sort, od, grep. read, never run.
set -u; LC_ALL=C; export LC_ALL

ONES="zero one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen"
TENS="x x twenty thirty forty fifty sixty seventy eighty ninety"
pick() { idx=$1; shift; j=0; for w in "$@"; do [ "$j" -eq "$idx" ] && { printf '%s' "$w"; return; }; j=$((j+1)); done; }
num2word() {
  n=$1
  if [ "$n" -eq 1000 ]; then printf 'one thousand'; return; fi
  if [ "$n" -ge 100 ]; then h=$((n/100)); r=$((n%100))
    printf '%s hundred' "$(pick "$h" $ONES)"; [ "$r" -gt 0 ] && printf ' %s' "$(num2word "$r")"; return; fi
  if [ "$n" -ge 20 ]; then t=$((n/10)); o=$((n%10))
    printf '%s' "$(pick "$t" $TENS)"; [ "$o" -gt 0 ] && printf '%s' "-$(pick "$o" $ONES)"; return; fi
  pick "$n" $ONES
}
# split a number-phrase into atoms: spaces AND hyphens break atoms
atoms_of() { printf '%s' "$(num2word "$1")" | sed 's/-/ /g'; }

# atoms_of already splits a number-phrase into atoms above.

# wav_dur: REAL duration in seconds from a PCM wav header (byte-rate + data chunk
# size). empty string when the atom is not yet recorded -- never a faked number.
wav_dur() {
  f="$1"; [ -f "$f" ] || { printf ''; return; }
  br=$(od -An -j28 -N4 -tu4 "$f" 2>/dev/null | tr -d ' ')
  doff=$(LC_ALL=C grep -abo -m1 data "$f" 2>/dev/null | head -1 | cut -d: -f1)
  { [ -n "$br" ] && [ -n "$doff" ] && [ "$br" -gt 0 ] 2>/dev/null; } || { printf ''; return; }
  sz=$(od -An -j$((doff+4)) -N4 -tu4 "$f" 2>/dev/null | tr -d ' ')
  [ -n "$sz" ] || { printf ''; return; }
  awk -v s="$sz" -v r="$br" 'BEGIN{printf "%.3f", s/r}'
}

case "${1:-}" in
  timeline)
    n="$2"; a=$(atoms_of "$n")
    echo "n=$n   words=\"$(num2word "$n")\""
    echo "the sound laid on the TIME AXIS (time is the spine, not a neighbor):"
    t=0; any=0
    for x in $a; do
      d=$(wav_dur "atoms/$x.wav")
      if [ -n "$d" ]; then
        end=$(awk -v a="$t" -v b="$d" 'BEGIN{printf "%.3f", a+b}')
        printf '  [%8.3f -> %8.3f s]  %s\n' "$t" "$end" "$x"; t="$end"; any=1
      else
        printf '  [    ?    ->    ?     s]  %s   (unrecorded -- the time-slot is real, the sound will fill it)\n' "$x"
      fi
    done
    [ "$any" -eq 1 ] && echo "  total: ${t}s -- this IS the number, written across time" \
                     || echo "  (no atoms recorded yet -- the axis is real and waiting; nothing faked)"
    ;;
  plan)
    n="$2"; w=$(num2word "$n"); a=$(atoms_of "$n")
    echo "n=$n   words=\"$w\""
    echo "atoms: $a"
    cmd="sox"; for x in $a; do cmd="$cmd atoms/$x.wav"; done; cmd="$cmd build/$n.wav"
    echo "stitch: $cmd"
    ;;
  atoms)
    # decompose EVERY number 0..1000, collect the unique atoms = the record list
    i=0; while [ "$i" -le 1000 ]; do atoms_of "$i"; echo; i=$((i+1)); done \
      | tr ' ' '\n' | sed '/^$/d' | sort -u > /tmp/atoms.list
    echo "the complete spoken-number vocabulary -- record each ONCE, real voice:"
    nl -w2 -s'. ' /tmp/atoms.list
    echo "TOTAL atoms to record: $(wc -l < /tmp/atoms.list)"
    ;;
  *) echo "usage: eden_compose_audio.sh plan <n> | timeline <n> | atoms"; exit 2;;
esac
