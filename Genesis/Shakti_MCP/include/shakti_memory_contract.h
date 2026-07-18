#ifndef SHAKTI_MEMORY_CONTRACT_H
#define SHAKTI_MEMORY_CONTRACT_H

#include <stdint.h>

#define SHAKTI_MEMORY_ID_MAX 128u
#define SHAKTI_MEMORY_TEXT_MAX 4096u
#define SHAKTI_MEMORY_TAG_MAX 64u
#define SHAKTI_CONVERGENCE_OBSERVATION_MAX 16u
#define SHAKTI_RECALL_BLOCK_MAX 8u

typedef enum {
    SHAKTI_EVIDENCE_OBSERVED = 0,
    SHAKTI_EVIDENCE_REPORTED,
    SHAKTI_EVIDENCE_INFERRED,
    SHAKTI_EVIDENCE_HYPOTHESIS,
    SHAKTI_EVIDENCE_UNKNOWN
} ShaktiEvidenceClass;

typedef enum {
    SHAKTI_MODALITY_TEXT = 0,
    SHAKTI_MODALITY_IMAGE,
    SHAKTI_MODALITY_AUDIO,
    SHAKTI_MODALITY_TOOL,
    SHAKTI_MODALITY_SYSTEM
} ShaktiModality;

typedef enum {
    SHAKTI_MEMORY_SCHOOL = 0,
    SHAKTI_MEMORY_LIVE
} ShaktiMemoryOrigin;

typedef enum {
    SHAKTI_MEMORY_EVENT = 0,
    SHAKTI_MEMORY_REFLECTION,
    SHAKTI_MEMORY_CORRECTION
} ShaktiMemoryKind;

typedef struct {
    char source_id[SHAKTI_MEMORY_ID_MAX];
    ShaktiModality modality;
    uint64_t called_epoch;
    uint64_t source_sequence;
    uint16_t confidence_milli;
    char canonical_text[SHAKTI_MEMORY_TEXT_MAX];
} ShaktiConvergenceObservation;

/* A measurable working-state candidate, not a claim of subjective feeling. */
typedef struct {
    ShaktiEvidenceClass evidence;
    int16_t valence_milli;
    uint16_t arousal_milli;
    uint16_t urgency_milli;
    uint16_t approach_milli;
    uint16_t avoidance_milli;
    uint16_t uncertainty_milli;
    uint16_t novelty_milli;
    char basis[SHAKTI_MEMORY_TEXT_MAX];
} ShaktiValuationSnapshot;

typedef struct {
    char frame_id[SHAKTI_MEMORY_ID_MAX];
    uint64_t called_epoch;
    uint16_t observation_count;
    ShaktiConvergenceObservation observations[SHAKTI_CONVERGENCE_OBSERVATION_MAX];
    ShaktiValuationSnapshot valuation;
} ShaktiConvergenceFrame;

/* Input to the future long-term-memory loader. Code supplies identity/time. */
typedef struct {
    char candidate_id[SHAKTI_MEMORY_ID_MAX];
    uint64_t source_task_epoch;
    ShaktiMemoryOrigin origin;
    ShaktiMemoryKind kind;
    ShaktiEvidenceClass evidence;
    uint64_t called_epoch;
    char canonical_text[SHAKTI_MEMORY_TEXT_MAX];
    char reflection_text[SHAKTI_MEMORY_TEXT_MAX];
    char convergence_frame_id[SHAKTI_MEMORY_ID_MAX];
    ShaktiValuationSnapshot valuation;
    char corrects_event_id[SHAKTI_MEMORY_ID_MAX];
} ShaktiReflectionCandidate;

/* Output from the future loader after validate, write, reopen, and verify. */
typedef struct {
    char candidate_id[SHAKTI_MEMORY_ID_MAX];
    char event_id[SHAKTI_MEMORY_ID_MAX];
    char storage_key[SHAKTI_MEMORY_ID_MAX];
    uint64_t committed_epoch;
    int committed;
    char detail[SHAKTI_MEMORY_TEXT_MAX];
} ShaktiMemoryReceipt;

/* Exact fields only. A later selector may add signed deterministic rules. */
typedef struct {
    char exact_event_id[SHAKTI_MEMORY_ID_MAX];
    char related_event_id[SHAKTI_MEMORY_ID_MAX];
    char tag[SHAKTI_MEMORY_TAG_MAX];
    uint64_t called_epoch_from;
    uint64_t called_epoch_to;
    uint16_t max_blocks;
} ShaktiRecallRequest;

typedef struct {
    char source_event_id[SHAKTI_MEMORY_ID_MAX];
    char short_term_text[SHAKTI_MEMORY_TEXT_MAX];
} ShaktiRecallBlock;

/* Read-only copies for one active cycle. Dismissal never edits the archive. */
typedef struct {
    char recall_set_id[SHAKTI_MEMORY_ID_MAX];
    uint16_t block_count;
    ShaktiRecallBlock blocks[SHAKTI_RECALL_BLOCK_MAX];
} ShaktiRecallSet;

#endif
