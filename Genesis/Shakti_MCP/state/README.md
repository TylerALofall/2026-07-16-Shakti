# File: `Genesis/Shakti_MCP/state/README.md`

Status: contract-only
Authority: MCP app-local runtime-state boundary
Version: 1
Last verified: 2026-07-17

## Introduction

The C core does not choose or escape the host application's storage sandbox.
The host owns persistence and may create app-local goal, notebook, inbox,
outbox, and pending-reflection directories in later route passes.

## Start and end points

Start: a confirmed router transition produces a pointer-free checkpoint or
typed durable-write request.
End: the host returns a verified receipt or explicit failure without granting
the router broader filesystem authority.

## Related documents

- `../HOST_CONTRACT.md` — assigns persistence to the host.
- `../FULL_PIPELINE.md` — separates temporary, staged, and immutable state.
- `../../../docs/MASTER_ARCHITECTURE.md` — project ownership and status map.

## Definitions

- **Checkpoint**: restart diagnostic state, not a claim of task completion.
- **Application sandbox**: the only filesystem scope available to the host adapter.
- **Pending reflection**: temporary candidate awaiting loader validation.

No runtime state is committed in this directory during the footing pass.
