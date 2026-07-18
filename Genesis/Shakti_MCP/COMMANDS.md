# File: `Genesis/Shakti_MCP/COMMANDS.md`

Status: implemented vocabulary; reserved routes labeled
Authority: Shakti and trusted-host command names
Version: 1
Last verified: 2026-07-17

## Introduction

The terminal text parser is a later host pass. The C footing currently receives
typed `route`, `operation`, and `payload` fields. Commands marked **reserved**
have real names and shapes but return `NOT_IMPLEMENTED` until their route pass.

## Start and end points

Start: an authenticated actor provides a typed command or trusted control.
End: the fixed route receives it, or the capability/actor boundary refuses it.

## Related documents

- `ROUTE_MAP.md` — identifies the owner and implementation state of each route.
- `HOST_CONTRACT.md` — separates trusted host control from model text.
- `FULL_PIPELINE.md` — explains when a command remains in-cycle or creates later work.

## Definitions

- **Available**: implemented in the current C footing.
- **Reserved**: stable syntax exists, but the route returns `NOT_IMPLEMENTED`.
- **Continue-type**: records a result without creating another model prompt.

## Shakti commands

| Command | State effect | Status |
|---|---|---|
| `epoch status` | Read the active task's frozen call time and cycle state. | available |
| `heartbeat status` | Read wake budget, wake schedule, runway, and stop state. | available |
| `continue` | Request one runway cycle for the unfinished task after this response completes. | available |
| `heartbeat off` | Cancel future wakes and runway, then drain active work. | available |
| `message <text>` | Optional Shakti-to-Tyler output; never controls completion. | available |
| `note <name> <text>` | Save a note, return a receipt, and continue the same cycle. No prompt and no heartbeat charge. | reserved |
| `notebook` | List note files and reminders. | reserved |
| `notebook <name>` | Read one note file into the same active cycle. | reserved |
| `reminder <epoch> <text>` | Stage a host reminder with an optional notebook reference. | reserved |
| `goal` | Read the current host-admitted goal. | reserved |
| `menu [tool [action args]]` | Traverse the staged tool menu one level at a time. | reserved |
| `shell <command>` | Request the external shell proxy through its capability gate. | reserved |
| `reflect <batch_id>` | Create a typed reflection candidate from one witnessed batch. | reserved |
| `remember <candidate_id>` | Ask the loader to validate and commit one candidate. | reserved |
| `recall id <event_id>` | Copy one immutable event block into short-term memory. | reserved |
| `recall related <event_id> <limit>` | Copy deterministically related blocks into short-term memory. | reserved |
| `recall dismiss <set_id>` | Discard copied recall blocks; long-term XML is unchanged. | reserved |

## Trusted host controls

```text
/status
/heartbeat on <budget> <delay_after_completion>
/heartbeat off
/result <operation_id> PASS|FAIL <text>
/tools open|restricted|drain|off
/mode rest|cruise|focus|reflect <seconds>|off
/stop
```

Host controls are not accepted from Shakti text. The future parser must establish
the trusted actor before constructing a `ShaktiCommand`.

`/mode cruise` is reserved for the later low-duty school scheduler; heartbeat
supplies its bounded wake pulse. `/mode reflect <seconds>` is the tool-free
quiet-reflection window. The router drain, capability refusal, and bounded
reflection pulse are implemented; the terminal command parser, monotonic host
window, and API request adapter remain unimplemented.
