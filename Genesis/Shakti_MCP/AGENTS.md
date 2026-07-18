# Shakti MCP component guidance

The MCP is a switchboard. It routes requests but does not contain tool
implementations, own the device clock, or grant device authority.

Required read order:

1. `../../docs/MASTER_ARCHITECTURE.md`
2. `FULL_PIPELINE.md`
3. `HOST_CONTRACT.md`
4. `ROUTE_MAP.md`
5. `COMMANDS.md`
6. `include/shakti_mcp.h` and `include/shakti_memory_contract.h`

Implementation rules:

- C17, fixed buffers, no heap in the router core.
- One cycle at a time. No stacked model prompts.
- `continue` preserves the original task's `called_epoch`; heartbeat and quiet
  reflection create new tasks using the admission call time.
- Quiet reflection drains active work, disables the capability proxy, and
  stages reflection only. It never auto-commits immutable memory.
- Long-term XML is append-only/read-only after commit. Recall returns a copy to
  temporary context and never mutates the stored block.
- Add or update tests for every state transition or refusal path.

Verification:

```sh
sh Genesis/Shakti_MCP/build.sh
sh docs/check_document_map.sh
```

Do not claim the component is pushed or merged unless the corresponding Git
commit and branch are verified.
