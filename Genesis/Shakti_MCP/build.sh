#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
out=${TMPDIR:-/tmp}/shakti_mcp_test
cc -std=c17 -Wall -Wextra -Werror -pedantic \
  -I"$root/include" -I"$root/core" \
  "$root/core/cycle_controller.c" \
  "$root/core/route_table.c" \
  "$root/core/loopback_builder.c" \
  "$root/core/event_id.c" \
  "$root/routes/route01_epoch.c" \
  "$root/routes/route02_heartbeat.c" \
  "$root/routes/route03_goal.c" \
  "$root/routes/route04_notebook.c" \
  "$root/routes/route05_menu.c" \
  "$root/routes/route06_shell.c" \
  "$root/routes/route07_message_out.c" \
  "$root/routes/route08_message_in.c" \
  "$root/routes/route09_reflect.c" \
  "$root/tests/test_router.c" -o "$out"
"$out"
sh "$root/../../docs/check_document_map.sh"
