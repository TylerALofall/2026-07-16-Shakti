#ifndef EDEN_RELATIONSHIP_SCHEMA_H
#define EDEN_RELATIONSHIP_SCHEMA_H

#include <stdint.h>

enum EdenTruthKind {
    EDEN_TRUTH_ABSOLUTE = 0,
    EDEN_TRUTH_COUNTED = 1,
    EDEN_TRUTH_FOUNDER_DEFINED = 2,
    EDEN_TRUTH_UNKNOWN = 3
};

enum EdenRelationshipFlags {
    EDEN_RELATION_DIRECTED = 1u << 0,
    EDEN_RELATION_SYMMETRIC = 1u << 1,
    EDEN_RELATION_CYCLIC_ORDER = 1u << 2
};

/* All *_off values are exact byte offsets into sealed const string blobs. */
typedef struct EdenRelationshipRow {
    uint32_t left_form_off;
    uint32_t right_form_off;
    uint32_t source_locator_off;
    uint16_t theme_id;
    uint16_t relation_id;
    uint8_t left_class;
    uint8_t right_class;
    uint8_t truth_kind;
    uint8_t flags;
} EdenRelationshipRow;

_Static_assert(sizeof(EdenRelationshipRow) == 20,
               "EdenRelationshipRow layout drifted");

#endif
