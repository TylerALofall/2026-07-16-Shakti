# File: `Genesis/Shakti_MCP/ROUTE_MAP.md`

Status: implemented fixed table; reserved handlers labeled
Authority: nine-route ownership and dispatch map
Version: 1
Last verified: 2026-07-17

## Introduction

The core owns one fixed nine-entry routing table. Routes never call one another
to create prompts. The host admits external triggers; command results remain in
the active cycle.

## Start and end points

Start: one validated `ShaktiCommand` reaches `shakti_dispatch()`.
End: exactly one handler returns an in-cycle result or explicit refusal.

## Related documents

- `COMMANDS.md` — text vocabulary mapped onto these fixed routes.
- `HOST_CONTRACT.md` — establishes trusted actor and cycle state before dispatch.
- `FULL_PIPELINE.md` — shows the switchboard between engine and capability gate.

## Definitions

- **Route**: one fixed ownership slot, not a tool implementation.
- **Handler**: the C function compiled into that slot.
- **Same-cycle result**: a return value that does not prompt the model again.

## Route table

| ID | Route | This pass |
|---:|---|---|
| 1 | epoch | Returns the task's frozen `called_epoch`, never a moving clock. |
| 2 | lifecycle | Runway `continue`, heartbeat status, and graceful `off`; host-only wake scheduling. |
| 3 | goal | Explicit `NOT_IMPLEMENTED` footing. |
| 4 | notebook | Explicit `NOT_IMPLEMENTED` footing for notes/reminders. |
| 5 | menu | Points to the staged XML schema; actions remain disabled. |
| 6 | shell | Transport stub; executes nothing. |
| 7 | message.out | Optional unchanged Shakti-to-Tyler response. |
| 8 | message.in | Host-owned and refused when called by Shakti. |
| 9 | reflect | Stub; stores nothing permanently. |

## Trigger directions

- Tyler input and reminders may start a host-admitted cycle.
- A heartbeat starts a new idle/cruise wake task only when the host schedule is
  armed and no cycle is active.
- `continue` starts one bounded runway cycle for the unfinished task and keeps
  that task's original call time.
- Quiet reflection is a controller state, not a tenth route. It drains the
  active cycle, closes the capability gate, and permits only status/lifecycle
  and staged-reflection route traffic during bounded recovery pulses.
- A tool result resumes its existing cycle through `shakti_post_result()`.
- Notes, command results, and `message.out` never create a new cycle.
- Route 7 is optional. Heartbeat budget is spent at admission, regardless of
  whether Shakti sends a message or calls a route.
