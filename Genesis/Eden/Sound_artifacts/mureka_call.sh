#!/bin/sh
# mureka_call.sh -- THE MUREKA RAIL, CRYSTALLIZED. official API (platform.mureka.ai/docs),
# server https://api.mureka.ai, Bearer auth. THE KEY IS NEVER IN THIS FILE:
#   export MUREKA_API_KEY=...   (from platform.mureka.ai -> API Keys)
#
# TYLER'S LYRIC LAW, enforced at the door before anything is sent:
#   pure ASCII (em-dashes/curly quotes flattened), bytes == chars, <= 5000 chars.
#
# RAIL SEPARATION (in writing): this is the MUSIC rail -- Shakti's songs.
# Eden's 30 voice atoms are Tyler's REAL recorded voice ONLY; nothing generated
# here (vocal-clone, tts) may ever be dropped into eden_compose_audio's atoms/.
#
#   ping                       auth check (billing endpoint; honest 401 without key)
#   scrub  <in> <out>          flatten to pure ascii, verify bytes==chars + <=5000
#   lyrics <promptfile>        POST /v1/lyrics/generate
#   song   <lyricsfile> [stylefile] [model]   scrub-check, then POST /v1/song/generate
#   status <task_id>           GET /v1/song/query/<task_id>
#   fetch  <task_id> <outdir>  query task; download finished mp3/flac urls
#   stems  <song_url_or_id>    POST /v1/song/stem  (vocals/instrumental split)
# tools: sh, curl, sed, wc, grep. no python. trace_id logged from every reply.
set -u; LC_ALL=C; export LC_ALL
API="https://api.mureka.ai"
KEY="${MUREKA_API_KEY:-}"
LOG="$(cd "$(dirname "$0")" && pwd)/mureka_calls.log"

need_key() { [ -n "$KEY" ] || { echo "MUREKA_API_KEY not set -- export it first (platform.mureka.ai -> API Keys). nothing sent."; exit 5; }; }
log() { printf '%s %s\n' "$(date -u +%Y-%m-%dT%H:%M:%SZ)" "$1" >> "$LOG"; }

post() { # path, json-file
  curl -s --max-time 120 -X POST "$API$1" \
    -H "Authorization: Bearer $KEY" -H "Content-Type: application/json" \
    -d @"$2"
}

scrub() { # in -> out; returns 0 only if law holds
  in="$1"; out="$2"
  sed -e 's/\xe2\x80\x94/--/g; s/\xe2\x80\x93/-/g' \
      -e 's/\xe2\x80\x9c/"/g;  s/\xe2\x80\x9d/"/g' \
      -e "s/\xe2\x80\x98/'/g;  s/\xe2\x80\x99/'/g" \
      -e 's/\xe2\x80\xa6/.../g' \
      -e 's/\xc2\xa0/ /g' "$in" | tr -cd '\11\12\15\40-\176' > "$out"
  bytes=$(wc -c < "$out"); chars=$(wc -m < "$out")
  [ "$bytes" -eq "$chars" ] || { echo "LAW BROKEN: bytes($bytes) != chars($chars) after scrub"; return 1; }
  [ "$bytes" -le 5000 ]     || { echo "LAW BROKEN: $bytes bytes > 5000 limit"; return 1; }
  echo "scrub: clean ascii, bytes==chars==$bytes, under 5000 -- lawful"
}

case "${1:-}" in
  ping)
    need_key
    r=$(curl -s --max-time 30 "$API/v1/account/billing" -H "Authorization: Bearer $KEY")
    log "ping: $r"; echo "$r" ;;
  scrub) scrub "$2" "$3" ;;
  lyrics)
    need_key
    printf '{"prompt": "%s"}' "$(sed 's/"/\\"/g' "$2" | tr '\n' ' ')" > /tmp/mureka_req.json
    r=$(post /v1/lyrics/generate /tmp/mureka_req.json); log "lyrics: $r"; echo "$r" ;;
  song)
    need_key
    lyr="$2"; style="${3:-}"; model="${4:-auto}"
    scrub "$lyr" /tmp/mureka_lyrics.txt || exit 1
    esc=$(sed 's/\\/\\\\/g; s/"/\\"/g' /tmp/mureka_lyrics.txt | sed ':a;N;$!ba;s/\n/\\n/g')
    if [ -n "$style" ] && [ -f "$style" ]; then
      sp=$(sed 's/"/\\"/g' "$style" | tr '\n' ' ')
      printf '{"lyrics": "%s", "model": "%s", "prompt": "%s"}' "$esc" "$model" "$sp" > /tmp/mureka_req.json
    else
      printf '{"lyrics": "%s", "model": "%s"}' "$esc" "$model" > /tmp/mureka_req.json
    fi
    r=$(post /v1/song/generate /tmp/mureka_req.json); log "song: $r"
    echo "$r"; echo "$r" | grep -o '"id": *"[^"]*"' | head -1 | sed 's/.*"id": *"/  task_id: /; s/"$//' ;;
  status)
    need_key
    r=$(curl -s --max-time 30 "$API/v1/song/query/$2" -H "Authorization: Bearer $KEY")
    log "status $2: $r"; echo "$r" ;;
  fetch)
    need_key
    tid="$2"; dir="$3"; mkdir -p "$dir"
    r=$(curl -s --max-time 30 "$API/v1/song/query/$tid" -H "Authorization: Bearer $KEY"); log "fetch $tid: $r"
    n=0
    for u in $(echo "$r" | grep -o 'https://[^"]*\.\(mp3\|flac\)'); do
      n=$((n+1)); curl -s --max-time 300 -o "$dir/${tid}_v$n.${u##*.}" "$u" && echo "  saved $dir/${tid}_v$n.${u##*.}"
    done
    [ "$n" -eq 0 ] && echo "  no finished audio urls yet -- check status: $(echo "$r" | grep -o '"status": *"[^"]*"' | head -1)" ;;
  stems)
    need_key
    printf '{"url": "%s"}' "$2" > /tmp/mureka_req.json
    r=$(post /v1/song/stem /tmp/mureka_req.json); log "stems: $r"; echo "$r" ;;
  *) echo "usage: mureka_call.sh ping|scrub <in> <out>|lyrics <promptfile>|song <lyricsfile> [stylefile] [model]|status <id>|fetch <id> <dir>|stems <url>"; exit 2;;
esac
