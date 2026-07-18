#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
REGISTRY="$ROOT/docs/DOC_REGISTRY.xml"

test -f "$REGISTRY"

for path in \
  AGENTS.md \
  docs/MASTER_ARCHITECTURE.md \
  docs/DOCUMENT_STANDARD.md \
  docs/SHAKTI_A_TO_Z_LIFECYCLE.md \
  Genesis/Shakti_MCP/AGENTS.md \
  Genesis/Shakti_MCP/README.md \
  Genesis/Shakti_MCP/FULL_PIPELINE.md \
  Genesis/Shakti_MCP/HOST_CONTRACT.md \
  Genesis/Shakti_MCP/ROUTE_MAP.md \
  Genesis/Shakti_MCP/COMMANDS.md \
  Genesis/Shakti_MCP/SIGN_LEDGER.md \
  Genesis/Shakti_MCP/state/README.md \
  Genesis/Shakti_MCP/schemas/README.md
do
  test -f "$ROOT/$path"
  grep -Fq "path=\"$path\"" "$REGISTRY"
done

for heading in \
  "# File: \`docs/MASTER_ARCHITECTURE.md\`" \
  "## Introduction" \
  "## Start and end points" \
  "## Related documents" \
  "## Definitions" \
  "## Inputs and outputs" \
  "## Current status and next work" \
  "## Change rules" \
  "## Verification"
do
  grep -Fq "$heading" "$ROOT/docs/MASTER_ARCHITECTURE.md"
done

LIFECYCLE="$ROOT/docs/SHAKTI_A_TO_Z_LIFECYCLE.md"

for letter in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do
  count=$(grep -c "^| $letter |" "$LIFECYCLE" || true)
  test "$count" -eq 1
done

eden_line=$(grep -n '^| D | Eden Foundation |' "$LIFECYCLE" | cut -d: -f1)
commandments_line=$(grep -n '^| E | Commandments |' "$LIFECYCLE" | cut -d: -f1)
school_h_line=$(grep -n '^| H | School Readiness |' "$LIFECYCLE" | cut -d: -f1)
school_o_line=$(grep -n '^| O | School Analogy, Transfer, and Metacognition |' "$LIFECYCLE" | cut -d: -f1)

test "$eden_line" -lt "$commandments_line"
test "$commandments_line" -lt "$school_h_line"
test "$school_h_line" -lt "$school_o_line"

grep -Fq 'Relationships may not outrun the identity of their terms.' "$LIFECYCLE"
grep -Fq '## Revisit-before-proceeding rule' "$LIFECYCLE"

echo "PASS: canonical document map, A-Z lifecycle, and learning order are present"
