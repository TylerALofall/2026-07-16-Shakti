# Shakti repository guidance

Before changing architecture, code, schemas, training artifacts, or project
status, read these files in order:

1. `docs/MASTER_ARCHITECTURE.md`
2. `docs/SHAKTI_A_TO_Z_LIFECYCLE.md`
3. `docs/DOCUMENT_STANDARD.md`
4. `docs/DOC_REGISTRY.xml`
5. The component README and every related document named by the registry.

Rules:

- Treat the master architecture and the nearest component contract as the
  authority. If a request conflicts with them, stop, identify the conflict, and
  ask whether the authority document should change. Do not silently improvise.
- Do not mix Eden, School, MCP routing, convergence, temporary memory, and
  immutable memory responsibilities. Follow the ownership boundaries in the
  master architecture.
- Eden precedes Commandments. School owns the dependency order for grounded
  symbols, words, relationships, classification, inference, and transfer.
- If a School prerequisite is missing, create a recorded revisit subsection
  before proceeding. Do not silently teach the later relationship first.
- Shakti architecture is engine-neutral. Do not redefine it as an LLM wrapper.
- Every new durable document must follow `docs/DOCUMENT_STANDARD.md` and be
  added to `docs/DOC_REGISTRY.xml` in the same change.
- Every architectural change must update the master architecture status table,
  the affected component document, and the registry's `last_verified` field.
- Mark behavior as `implemented`, `contract-only`, `reserved`, `blocked`, or
  `deprecated`. Never describe a stub or schema as working code.
- Preserve append-only history. Corrections point to earlier decisions; they do
  not erase them.
- Do not move task time. Each task keeps the wall epoch captured when it was
  admitted. Scheduling clocks are eligibility signals, not event identity.
- Models provide content and bounded choices. Host code creates identifiers,
  timestamps, paths, serialization, permissions, and durable writes.
- Tool access is host-controlled. A model request cannot expand its own
  capabilities.

For MCP work, also read `Genesis/Shakti_MCP/AGENTS.md`.
