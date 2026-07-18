# File: `Genesis/Shakti_MCP/README.md`

Status: implemented footing
Authority: MCP component entry point and boundary map
Version: 1
Last verified: 2026-07-17

## Introduction

This is a compact C17 switchboard for message and tool routing. It is not Eden,
a tool implementation, a permanent-memory system, an operating-system service,
or an autonomous timer.

## Start and end points

Start: the host admits one typed task with a frozen `called_epoch`, bounded
context, and an explicit capability policy.
End: the router returns one same-cycle result, a waiting operation ID, a
completed/drained state, or one bounded loopback bundle. Durable storage and
actuator execution happen beyond the router through host-owned interfaces.

## Related documents

- `../../docs/MASTER_ARCHITECTURE.md` — project-wide topology, ownership, and
  completion table; read this first.
- `FULL_PIPELINE.md` — detailed flow from engine through convergence,
  reflection, XML loading, and recall.
- `HOST_CONTRACT.md` — exact boundary between the C router and the host.
- `ROUTE_MAP.md` — ownership and state of all nine routes.
- `COMMANDS.md` — model and host command vocabulary.

## Definitions

- **Heartbeat**: a bounded host-scheduled wake that begins a new task.
- **Runway**: a bounded continuation of the current unfinished task.
- **Quiet reflection**: a host-controlled recovery mode with the tool proxy off.
- **Footing**: tested routing/state behavior that other modules can build on;
  it does not mean the reserved routes are implemented.

## Component mapping

| Start | Owner | End | Detailed contract |
|---|---|---|---|
| admitted task | cycle controller | running/waiting/completed state | `HOST_CONTRACT.md` |
| model command | fixed route table | same-cycle result or refusal | `ROUTE_MAP.md` |
| unfinished task | runway controller | one continuation bundle | `FULL_PIPELINE.md` |
| scheduled idle wake | heartbeat controller | one activity/reflection bundle | `FULL_PIPELINE.md` |
| recovery request | quiet-reflection controller | drained, tool-free reflection | `FULL_PIPELINE.md` |
| reflection candidate | future loader boundary | receipt or refusal | `include/shakti_memory_contract.h` |
| recall request | future memory reader | temporary copied blocks | `include/shakti_memory_contract.h` |

## Frozen task time

The host supplies `called_epoch` once when it admits a cycle. The router freezes
that value. Every address and route result inside that task uses the same time;
no route reads or moves a clock. The host may supply a later `now_epoch` to
`shakti_pump()` only to decide whether an already-armed continuation is eligible.
Admission of that continuation creates a new task with the supplied call time.

## Event address and firing order

The visible event ID is `<called_epoch>-<circuit>-<firing_order>`. It does not
contain the internal diagnostic cycle counter.

- `1784311200-A-1` — circuit A, first function fired under that task epoch.
- `1784311200-Aa-2` — subsystem a of circuit A, second function fired.
- `1784311200-Ab-3` — subsystem b of circuit A, third function fired.

The C router assigns the final number; Shakti never types it. The firing counter
continues across serial continuations or serial cycles sharing the same frozen
epoch, so functions retain one traceable order without implying seven cycles
were running. The current controller admits only one cycle at a time. If future
parallel engines are added, they require an explicit host-instance namespace;
the cycle counter must not be smuggled back into this event address.

## Heartbeat wake and bounded runway

A heartbeat is a host-scheduled wake invitation for homework, chores, music,
chosen activity, and reflection. It starts a new task and is charged when its
cycle is admitted. The default development wake budget is nine, heartbeat boots
off, and the budget never silently refills.

Runway is separate. During active work Shakti may call `continue` to request one
more response for the unfinished task. That continuation preserves the task's
original `called_epoch` and spends its own bounded runway budget.

`shakti_request_stop()` cancels future continuation immediately but never kills
active work. The current cycle and any real pending result reach completion,
after which the router remains idle with heartbeat off. Persistence belongs to
the host device's application sandbox.

`shakti_checkpoint_capture()` exposes a pointer-free snapshot after each
confirmed transition. The host—not Shakti—writes that snapshot using ordinary
app-local storage. A snapshot taken during `WAITING_FOR_RESULT` retains the
exact cycle and operation IDs, so an unexpected device stop is recovered as
unfinished/unknown rather than silently rewritten as success or failure.

## External result door

A host attachment posts a completed operation through `shakti_post_result()`.
The operation ID must match the one pending in the active cycle. The result
resumes that same cycle and cannot change its `called_epoch` or identity.

The core is transport-neutral. A future OpenAI host adapter may map this to the
Responses API function-call/output continuation pattern; no API client,
credential, network access, or response storage is implemented here:
https://developers.openai.com/api/docs/guides/tools-programmatic-tool-calling

## Build and test

```text
sh Genesis/Shakti_MCP/build.sh
```

Windows uses `build_clang.bat`. Both compile every route explicitly under
strict C17 and run the deterministic footing tests.

The POSIX build also checks that the canonical documentation map is present and
that every MCP authority document is registered.

## Build-forward contracts

`FULL_PIPELINE.md` maps engine, router, capability gate, actuator, convergence,
reflection, XML loading, selective recall, and short-term copies. The XSDs and
examples under `schemas/` plus `include/shakti_memory_contract.h` mark their
input/output shapes. They are contracts only; Routes 4, 5, 6, and 9 remain
honest stubs until their individual passes are built and tested.
