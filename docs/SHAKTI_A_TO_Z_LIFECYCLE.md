# File: `docs/SHAKTI_A_TO_Z_LIFECYCLE.md`

Status: architecture contract
Authority: global Shakti lifecycle and section lettering
Version: 1
Last verified: 2026-07-17

## Introduction

This document follows one datum from activation, through Eden, School, live
action, reflection, memory storage, and sleep. Shakti is not defined as an LLM.
She is an engine-neutral learning system whose inference engine, deterministic
components, sensors, and future substrates are replaceable participants. None
of those participants defines this lifecycle.

Eden is stateless ground. Commandments come after Eden and govern the learner
that can now receive, act, and remember. School owns the developmental order of
symbols, words, relationships, classification, inference, and transfer.

## Start and end points

Start: the first datum enters an inactive Shakti host.
End: the completed task is checkpointed, any accepted memory is stored
immutably, temporary context is released, and Shakti waits asleep for a real
message, reminder, or bounded heartbeat wake.

## Related documents

- `MASTER_ARCHITECTURE.md` — high-level component ownership and implementation status.
- `../Genesis/Eden/README.md` — stateless Eden ground, tablets, and prerequisite order.
- `../Genesis/Eden/LAWS.md` — current binding Eden laws.
- `../Genesis/Shakti_MCP/FULL_PIPELINE.md` — live routing, reflection, storage, and recall contracts.
- `DOCUMENT_STANDARD.md` — required structure and update rules for project documents.

## Definitions

- **Ground**: a representation connected to an observable signal, action,
  exact identity, or already demonstrated prerequisite.
- **School**: the ordered learning and performance-evidence system after Eden.
- **Relationship**: one precisely named connection between already grounded forms.
- **Inference**: a bounded conclusion supported by grounded facts and an
  admitted rule; missing support produces `UNKNOWN`.
- **Revisit**: a recorded return to the nearest missing prerequisite before a
  later section proceeds.
- **Firing order**: the C-assigned sequence of functions under one frozen task epoch.

## Global A–Z lifecycle

| Section | Name | Purpose and activity | Entry | Exit evidence |
|---|---|---|---|---|
| A | Activation | Admit the first datum, start the host process, initialize fixed state, and create no assumed meaning. | Physical or software start signal. | Core storage exists in a known empty state. |
| B | Boot Integrity | Verify executable identity, schemas, fixed capacities, required files, and last checkpoint without rewriting uncertain history. | A completed. | Required components pass or boot refuses with an exact reason. |
| C | Device Boundary | Establish the application sandbox, capability proxy, actor permissions, and OFF/RESTRICTED/DRAIN/OPEN control states. | B passed. | No later component can widen its own authority. |
| D | Eden Foundation | Load the stateless exact ground: fixed symbols, quantities, words, sensory slots, tablets, and prerequisite index. Eden does not remember the learner. | C boundary exists. | Every admitted Eden item is exact, ordered, receipted, and callable without live memory. |
| E | Commandments | Apply the enduring laws governing truth, uncertainty, consent, corrections, tools, memory, and conduct after Eden has supplied usable forms. | D foundation available. | Every later decision is constrained by named laws; conflicts refuse or escalate. |
| F | Frozen Epoch | Capture the task's call epoch once, establish its root identity, and initialize the universal firing order for that epoch. | A real trigger is admitted. | The task time cannot move; visible events use `epoch-circuit-order`. |
| G | Gateway Intake | Preserve Tyler's message or another admitted input verbatim, establish actor/source, normalize the envelope, and reject overflow whole. | F identity exists. | One bounded input envelope enters School and working context. |
| H | School Readiness | Regulate activity, orient attention, check access and safety, and activate only the directly relevant prior ground. | G input exists. | The learner can attend to the target; otherwise the task rests or revisits. |
| I | School Sensory Grounding | Align congruent sight, sound, touch, motion, timing, action, and result; establish object persistence, repetition, and cause/effect before abstraction. | H ready. | The target can be recognized or acted on across a small variation. |
| J | School Symbol Identity | Bind exact marks, sounds, gestures, pictures, numerals, letters, and stable referents without yet asserting broad relationships. | I grounded signal. | The same form is recognized and distinguished from another form. |
| K | School Words, Quantities, and Referents | Ground Eden words and quantities in their observed objects, actions, sets, attributes, or definitions; establish vocabulary before word-based relationships. | J symbol identity. | The learner can name, select, produce, or count the referent accurately. |
| L | School Direct Relationships | Teach one named relationship at a time between grounded forms: identity/case/same quantity, comparison, part/whole, membership, containment, location, sequence, role, life stage, conversion, and action/result. | Both sides are grounded in K or earlier. | The learner identifies direction, relation type, and limits without a vague association. |
| M | School Classification and Computed Views | Place direct facts into precise themes and classes, then compute lesson views by filtering, joining, ordering, and formulas instead of storing an all-pairs graph. | L relationship facts admitted. | Classification predicts the correct members and refuses mixed or unsupported classes. |
| N | School Ordered Inference | Use observed facts plus admitted rules to predict, deduce, sequence, compare, explain cause, test contradiction, and return TRUE/FALSE/UNKNOWN. | Required K–M ground demonstrated. | The learner shows the facts and rule used; missing evidence remains named. |
| O | School Analogy, Transfer, and Metacognition | Map the same deep relationship across changed surface forms, choose among strategies, test the result, reflect on errors, and schedule a revisit. | N inference works in its taught setting. | The relationship transfers to a new context with an explanation and calibrated confidence. |
| P | Working Memory and Selective Recall | Assemble the current message, Eden/Commandment anchors, relevant School evidence, live history, goal, and read-only copies recalled from immutable memory. | School routing identifies needed context. | One bounded current-context block exists; archive sources remain unchanged. |
| Q | Sensory Convergence | Align current modalities and real tool/result observations into one time-bounded frame while preserving disagreement and evidence class. | P context plus current observations. | One validated convergence frame or an explicit incomplete/contradictory state. |
| R | Grounded Deliberation | Apply the inference and transfer methods learned in N–O to choose a bounded next step, continue, ask, rest, reflect, or refuse. | P and Q available. | One intention with stated ground, uncertainty, and stopping condition. |
| S | MCP Switchboard | Convert the intention into one typed route, operation, and payload without embedding the tool implementation. | R intention exists. | One fixed route accepts it or returns a named refusal. |
| T | Permission Gate | Check actor, mode, route, capability, safety state, and tool proxy policy before any external effect. | S typed request. | `ALLOW`, `REFUSE`, or `DRAIN`, with an operation ID only for allowed work. |
| U | Tool or Actuator Execution | Perform the permitted file, shell, sensory, music, network, UI, or device operation outside the router's authority boundary. | T allowed operation. | Real `PASS`/`FAIL` result and payload; no fabricated success. |
| V | Result Re-entry | Match the operation ID, return the result to the same frozen task, update convergence and working history, and prevent a stacked prompt. | U real result. | The original cycle resumes with attributable evidence. |
| W | Response and Task Completion | Communicate optionally, finish the bounded task, record final status, and choose complete, continue, wait, drain, or quiet reflection. | No unresolved active operation. | Completion/refusal/wait state is explicit; output is not required for completion. |
| X | Self-Reflection and Memory Candidate | Review witnessed actions, results, errors, choices, and valuation evidence; create a staged candidate without rewriting history or auto-committing it. | W reaches a reflective boundary. | Typed candidate points to its source events and evidence class. |
| Y | Validation, Immutable Write, and Checkpoint | Validate the candidate, serialize canonical text/XML, write append-only, reopen and verify, issue a receipt, checkpoint runtime state, then release temporary recall copies. | X candidate or no-memory completion. | Durable receipt or explicit refusal; archive blocks are read-only after commit. |
| Z | Sleep and Heartbeat Wait | Close capabilities, stop inference, retain only allowed durable state, and wait for a message, reminder, or bounded host-scheduled wake. | Y checkpoint complete. | No active cycle; next wake begins again at F with a new frozen epoch. |

## School dependency order

School follows this sequence even when domains develop in parallel:

1. H — readiness and attention.
2. I — regulated sensory event and intentional action/result.
3. J — stable reference and symbol identity.
4. K — word, quantity, object, action, and definition grounding.
5. L — one precise direct relationship between grounded forms.
6. M — theme/class membership and computed relationship views.
7. N — evidence-bound prediction and inference.
8. O — variation, analogy, transfer, strategy choice, and reflection.

The learning cycle inside each admissible School node is:

`orient → activate prerequisite → predict → encode one relationship → align senses → generate → retrieve → correct → space → vary → interleave → transfer → reflect`.

## Relationship order inside School

Relationships may not outrun the identity of their terms. The default order is:

1. Exact identity and discrimination.
2. Symbol/referent and word/referent.
3. Same/different, case pair, one-to-one, and same quantity.
4. Comparison and ordered position.
5. Part/whole, contains, member-of, and located-in.
6. Precedes/follows and action/result.
7. Young-name, life-stage, role-counterpart, and other themed class facts.
8. Conversion, composition, and transformation under an exact rule.
9. Computed class views and multi-fact inference.
10. Analogy and contextual transfer.

One row remains one finite classification fact. Runtime graph views are
computed; unsupported associations remain outside Ground.

## Revisit-before-proceeding rule

Every School section begins with a prerequisite evidence check. If a later
section is reached early:

1. Pause the requested section; do not guess the missing ground.
2. Create a lowercase subsection under the current section, such as `Na`, whose
   metadata names the missing target, for example `revisit_target="K"`.
3. Load the nearest missing prerequisite and its grounding representation.
4. Retrieve or demonstrate it before showing the later relationship again.
5. Record PASS, FAIL, or UNKNOWN and the exact evidence.
6. Resume the parent section only after PASS. FAIL or UNKNOWN schedules another
   grounded revisit and never becomes an inferred success.

The visible firing order remains numeric: `epoch-Na-4` means the fourth
function fired under the task epoch occurred in N's first revisit subsection.

## Inputs and outputs

Input: one physical, sensory, message, reminder, result, or bounded wake datum.
Output: an explicit action/result and optional immutable memory receipt, followed
by a clean inactive checkpoint.

## Current status and next work

This A–Z map is the architectural naming contract. Eden exists as a stateless
foundation. The MCP footing implements part of F, S, V, W, Y checkpointing, and
Z control. School H–O, host adapters, convergence Q, tool execution U, and the
X–Y memory loader/commit path remain to be built or connected.

## Change rules

- Commandments remain after Eden.
- Inference and relationship learning remain owned by School.
- A runtime engine may use School learning but may not redefine its order.
- Missing prerequisites cause a recorded revisit, not a silent shortcut.
- Architecture is not tied to an LLM or vendor API.
- Any letter rename requires migration of event IDs, route documentation, the
  master architecture, and the document registry in one reviewed change.

## Verification

- Confirm every A–Z section appears once.
- Confirm D precedes E and H–O remain contiguous.
- Confirm every relationship stage depends on grounded identities.
- Confirm `sh docs/check_document_map.sh` passes.
