#include "shakti_mcp.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static ShaktiStatus append_text(char *out, size_t out_size, size_t *used,
                                const char *text) {
    size_t length = strlen(text);
    if (*used + length + 1u > out_size) return SHAKTI_TOO_LARGE;
    memcpy(out + *used, text, length + 1u);
    *used += length;
    return SHAKTI_OK;
}

static ShaktiStatus render_letters(uint16_t value, char base,
                                   char *out, size_t out_size) {
    char reversed[4];
    size_t count = 0u;
    size_t i;
    if (value == 0u || out == NULL || out_size == 0u) return SHAKTI_BAD_ARGUMENT;
    while (value > 0u && count < sizeof reversed) {
        --value;
        reversed[count++] = (char)(base + value % 26u);
        value = (uint16_t)(value / 26u);
    }
    if (value != 0u || count + 1u > out_size) return SHAKTI_TOO_LARGE;
    for (i = 0u; i < count; ++i) out[i] = reversed[count - i - 1u];
    out[count] = '\0';
    return SHAKTI_OK;
}

ShaktiStatus shakti_make_event_address(ShaktiRouter *router,
                                       const uint16_t *circuit_path,
                                       uint8_t circuit_depth,
                                       ShaktiEventAddress *address) {
    size_t i;
    if (router == NULL || address == NULL || circuit_path == NULL ||
        circuit_depth == 0u || circuit_depth > SHAKTI_CIRCUIT_DEPTH_MAX ||
        router->cycle.state == SHAKTI_CYCLE_IDLE ||
        router->event_epoch != router->cycle.called_epoch ||
        router->event_firing_sequence == UINT32_MAX)
        return SHAKTI_BAD_ARGUMENT;
    memset(address, 0, sizeof *address);
    address->called_epoch = router->cycle.called_epoch;
    address->circuit_depth = circuit_depth;
    for (i = 0u; i < circuit_depth; ++i) {
        if (circuit_path[i] == 0u) return SHAKTI_BAD_ARGUMENT;
        address->circuit_path[i] = circuit_path[i];
    }
    ++router->event_firing_sequence;
    address->firing_order = router->event_firing_sequence;
    ++router->transition_sequence;
    return SHAKTI_OK;
}

ShaktiStatus shakti_format_event_id(const ShaktiEventAddress *address,
                                    char *out, size_t out_size) {
    char piece[32];
    size_t used = 0u;
    size_t i;
    int written;
    ShaktiStatus status;
    if (address == NULL || out == NULL || out_size == 0u ||
        address->called_epoch == 0u || address->firing_order == 0u ||
        address->circuit_depth == 0u ||
        address->circuit_depth > SHAKTI_CIRCUIT_DEPTH_MAX)
        return SHAKTI_BAD_ARGUMENT;
    written = snprintf(piece, sizeof piece, "%" PRIu64 "-",
                       address->called_epoch);
    if (written < 0 || (size_t)written >= sizeof piece) return SHAKTI_TOO_LARGE;
    out[0] = '\0';
    status = append_text(out, out_size, &used, piece);
    if (status != SHAKTI_OK) return status;
    for (i = 0u; i < address->circuit_depth; ++i) {
        status = render_letters(address->circuit_path[i],
                                i == 0u ? 'A' : 'a', piece, sizeof piece);
        if (status != SHAKTI_OK) return status;
        status = append_text(out, out_size, &used, piece);
        if (status != SHAKTI_OK) return status;
    }
    written = snprintf(piece, sizeof piece, "-%" PRIu32,
                       address->firing_order);
    if (written < 0 || (size_t)written >= sizeof piece) return SHAKTI_TOO_LARGE;
    status = append_text(out, out_size, &used, piece);
    if (status != SHAKTI_OK) return status;
    return SHAKTI_OK;
}
