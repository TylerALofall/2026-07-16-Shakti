# File: `Genesis/Shakti_MCP/schemas/README.md`

Status: contract-only
Authority: convergence and long-term-memory XML shapes
Version: 1
Last verified: 2026-07-17

## Introduction

`memory_event_v1.xsd` and `convergence_frame_v1.xsd` are build-forward schema
contracts. The files under `examples/` must validate against them.

## Start and end points

Start: a typed C candidate or convergence frame reaches serialization.
End: XML validates against exactly one named schema, or is refused whole.

## Related documents

- `../FULL_PIPELINE.md` — loader, commit, and recall workflows.
- `../include/shakti_memory_contract.h` — fixed-buffer C boundary shapes.
- `../../../docs/MASTER_ARCHITECTURE.md` — project status and ownership map.

## Definitions

- **Schema**: validation contract, not persistence code.
- **Event ID**: `<epoch>-<circuit/subsystem>-<firing order>`.
- **Immutable**: API and storage policy prevent rewriting after validated commit.

These schemas are separate from Eden tablet XML. A tablet is curriculum input;
a memory event is an immutable witnessed-event record; a convergence frame is
a bounded temporary alignment of inputs. None may be accepted under another's
schema.

Storage and recall are not implemented merely because these schemas exist.
Route 9 remains `NOT_IMPLEMENTED` until the loader and recall passes satisfy the
workflow and crash-safety laws in `FULL_PIPELINE.md`.

The examples use IDs such as `17939201-Ca-2` and `17939201-Ra-3`. Internal
diagnostic cycle counters are not part of serialized event identity.
