# File: `Genesis/Shakti_MCP/HOST_CONTRACT.md`

Status: implemented router boundary; host adapter pending
Authority: host/router ownership and cycle protocol
Version: 1
Last verified: 2026-07-17

## Introduction

The router does not own a clock, timer, filesystem, model session, tool, or
device privilege. The host application owns those boundaries and follows this
single-cycle contract.

## Start and end points

Start: the host captures task call time and admits one cycle.
End: the host checkpoints a confirmed transition and owns any external result,
storage write, model request, or proxy action that follows.

## Related documents

- `README.md` — component boundary and event-address summary.
- `FULL_PIPELINE.md` — complete build-forward topology.
- `COMMANDS.md` — trusted controls and model command vocabulary.

## Definitions

- **Host**: trusted application code outside the router core.
- **Admission**: the only transition that creates a new active model task.
- **Drain**: refuse new actions while allowing real active work to close.
- **Eligibility time**: scheduling input only; it does not replace task identity.

## Cycle protocol

1. Admit exactly one external task with `shakti_admit_cycle()`. Pass the wall
   epoch captured when that task was called. That value becomes immutable task
   identity.
2. Give Shakti the task and any current short-term context. Route zero or more
   commands through `shakti_dispatch()`. Route 7 is optional.
3. If an external operation is started, assign its host-owned operation ID with
   `shakti_mark_operation_pending()`. At turn end, `shakti_turn_closed()` moves
   the cycle to `WAITING_FOR_RESULT`.
4. Return the real result through `shakti_post_result()`. It resumes the same
   cycle and inherits the original `called_epoch`.
5. If more runway is needed, Shakti requests `continue`. Confirmed completion
   may then arm one bounded continuation that preserves the task's original
   `called_epoch`.
6. Heartbeat is separate: enable and schedule a bounded wake invitation for an
   idle router. Call `shakti_pump(now_epoch)` from the host event loop.
   `now_epoch` checks scheduling eligibility only. If a heartbeat wake is
   admitted, that call's value is frozen as the new task's `called_epoch`.
7. Capture `shakti_checkpoint_capture()` after confirmed transitions and save
   it in the host application's local sandbox. The core neither requests nor
   receives wider device access.
8. For recovery, call `shakti_quiet_reflection_begin()`. If work is active, let
   it drain and checkpoint the completed boundary. Then schedule bounded
   reflection pulses with `shakti_quiet_reflection_schedule()`. Construct those
   model requests without tool definitions, keep the external capability proxy
   closed, and call `shakti_quiet_reflection_end()` when the host's monotonic
   recovery deadline is reached. Every admitted reflection still receives a
   frozen wall-clock `called_epoch` captured at admission.

`shakti_request_stop()` is a drain request: it cancels future continuation at
once, permits the active task and its real pending result to finish, and then
leaves the router idle with heartbeat off. A host may still provide a separate
emergency process kill; the last local checkpoint must then be treated as
unfinished or unknown, never rewritten as a clean completion.
