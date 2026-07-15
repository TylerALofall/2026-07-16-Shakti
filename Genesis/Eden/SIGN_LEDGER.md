# SIGN LEDGER — APPEND ONLY

Every changed path gets its own line. Wildcards and grouped directory entries
are refused after this contract correction.

Format:
`- YYYY-MM-DD | signer | ADDED|REPLACED|REMOVED | exact path | replaces: exact file or none`

## PRESERVED GENESIS HISTORY
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/README.md | replaces: none
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/LAWS.md | replaces: none
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/MODEL_INSTRUCTIONS.md | replaces: none
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/Tablet_Training_Artifacts_Master/* (legacy grouped entry) | replaces: none
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/Visual_Artifacts/gen_visual_artifacts.sh + seeded cards (legacy grouped entry) | replaces: none
- 2026-07-16 | Claude-Fable-5 | ADDED | Genesis/Eden/Sound_artifacts/lvl1_counting_{one..nine,zero}.wav (legacy grouped entry) | replaces: none

## CONTRACT CORRECTION
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/README.md | replaces: Genesis/Eden/README.md
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/LAWS.md | replaces: Genesis/Eden/LAWS.md
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/MODEL_INSTRUCTIONS.md | replaces: Genesis/Eden/MODEL_INSTRUCTIONS.md
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/SIGN_LEDGER.md | replaces: Genesis/Eden/SIGN_LEDGER.md
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/Tablet_Training_Artifacts_Master/00_READ_FIRST_RULES_AND_LAWS.md | replaces: Genesis/Eden/Tablet_Training_Artifacts_Master/00_READ_FIRST_RULES_AND_LAWS.md
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/Tablet_Training_Artifacts_Master/INDEX_MASTER.xml | replaces: Genesis/Eden/Tablet_Training_Artifacts_Master/INDEX_MASTER.xml
- 2026-07-15 | Codex | REPLACED | Genesis/Eden/Tablet_Training_Artifacts_Master/TEMPLATE_tablet.xml | replaces: Genesis/Eden/Tablet_Training_Artifacts_Master/TEMPLATE_tablet.xml
- 2026-07-15 | Codex | ADDED | Genesis/Eden/Tablet_Training_Artifacts_Master/EXAMPLE_L1_counting_ones_0002_2026-07-15.xml | replaces: Genesis/Eden/Tablet_Training_Artifacts_Master/EXAMPLE_L1_counting_ones_0001_2026-07-15.xml
- 2026-07-15 | Codex | REMOVED | Genesis/Eden/Tablet_Training_Artifacts_Master/EXAMPLE_L1_counting_ones_0001_2026-07-15.xml | replaces: none
- 2026-07-15 | Codex | ADDED | Genesis/Eden/Tablet_Training_Artifacts_Master/EXAMPLE_L1_counting_twos_0001_2026-07-15.xml | replaces: none
- 2026-07-15 | Codex | REMOVED | Genesis/Eden/Tablet_Training_Artifacts_Master/EXAMPLE_L2_alphabet_a_0001_2026-07-15.xml | replaces: none
