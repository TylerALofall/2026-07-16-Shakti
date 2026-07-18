# File: `docs/DOCUMENT_STANDARD.md`

Status: implemented
Authority: project-wide documentation contract
Version: 1
Last verified: 2026-07-17

## Introduction

This standard gives every Shakti document the same shape so a person or model
can find its purpose, authority, boundaries, dependencies, inputs, outputs, and
unfinished work without reconstructing the project from conversation history.

## Start and end points

Start: a durable fact, design, interface, procedure, or status needs to be
recorded.
End: the document follows this structure, appears in `DOC_REGISTRY.xml`, and
the master architecture links to it when it changes project topology or status.

## Related documents

- `MASTER_ARCHITECTURE.md` — canonical high-level topology and completion map.
- `DOC_REGISTRY.xml` — machine-readable filenames, authority, relationships,
  and verification state.
- `../AGENTS.md` — rules that make models read and maintain these documents.

## Definitions

- **Authority**: the scope in which this document wins when descriptions differ.
- **Contract-only**: shapes and laws exist, but runtime behavior is unfinished.
- **Implemented**: code or data exists and its named verification passes.
- **Reserved**: a stable name or route exists without its final behavior.
- **Start point**: the event or input where this document's responsibility begins.
- **End point**: the output or handoff where its responsibility stops.

## Required document structure

Every new durable Markdown document uses these sections in this order:

1. `# File: path` — exact repository-relative filename.
2. Metadata — Status, Authority, Version, Last verified.
3. Introduction.
4. Start and end points.
5. Related documents — each link gets one or two lines explaining why it
   matters.
6. Definitions — only terms required to read this document.
7. Contract, procedure, architecture, or content.
8. Inputs and outputs.
9. Current status and next work.
10. Change rules.
11. Verification.

Short documents may combine items 7–11, but may not omit filename, metadata,
introduction, start/end, related documents, or definitions.

## Naming and style

- Use uppercase snake case for authority documents: `HOST_CONTRACT.md`.
- Use `README.md` only as the entry point for its directory.
- Use repository-relative paths everywhere.
- One term has one spelling. Add project-wide terms to the master glossary.
- State facts directly. Label hypotheses and future work explicitly.
- Do not use “done” for a schema, stub, test fixture, or unconnected module.

## Inputs and outputs

Input: a proposed or changed durable document.
Output: a consistently structured file plus a registry entry and, when
architectural, an updated master map.

## Current status and next work

The standard is active for new documents. Existing documents are migrated when
they are materially edited; registry coverage is enforced immediately for the
MCP component and expands component by component.

## Change rules

Changing this standard requires updating `MASTER_ARCHITECTURE.md`,
`DOC_REGISTRY.xml`, and the document-map validation test in the same change.

## Verification

Run `sh docs/check_document_map.sh` from the repository root.
