#include "eden_clock.h"

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>

static const uint32_t unit_seconds[] = {
    1u, 60u, 3600u, 86400u
};

int eden_clock_tables_valid(void) {
    return sizeof unit_seconds / sizeof unit_seconds[0] == 4u &&
           unit_seconds[EDEN_TIME_SECOND] == 1u &&
           unit_seconds[EDEN_TIME_MINUTE] == EDEN_SECONDS_PER_MINUTE &&
           unit_seconds[EDEN_TIME_HOUR] == EDEN_SECONDS_PER_HOUR &&
           unit_seconds[EDEN_TIME_DAY] == EDEN_SECONDS_PER_DAY &&
           EDEN_CLOCK_SEMANTIC_POSITIONS == 132 &&
           EDEN_SECONDS_PER_CYCLE == 43200;
}

void eden_clock_state(uint64_t total_seconds, EdenClockState *out) {
    uint32_t cycle;
    uint32_t hour;
    if (!out) return;
    cycle = (uint32_t)(total_seconds % EDEN_SECONDS_PER_CYCLE);
    hour = cycle / EDEN_SECONDS_PER_HOUR;
    out->cycle_second = cycle;
    out->second_position = (uint8_t)(cycle % EDEN_SECONDS_PER_MINUTE);
    out->minute_position =
        (uint8_t)((cycle / EDEN_SECONDS_PER_MINUTE) % EDEN_MINUTES_PER_HOUR);
    out->hour_position = (uint8_t)hour;
    out->display_hour = (uint8_t)(hour == 0u ? 12u : hour);
    out->minute = out->minute_position;
    out->second = out->second_position;
    out->hour_subposition =
        (uint16_t)((cycle / EDEN_SECONDS_PER_MINUTE) % 720u);
}

int eden_time_convert_exact(uint64_t value, EdenTimeUnit from,
                            EdenTimeUnit to, uint64_t *result) {
    uint64_t seconds;
    uint32_t from_scale;
    uint32_t to_scale;
    if (!result || !eden_clock_tables_valid() || from < EDEN_TIME_SECOND ||
        from > EDEN_TIME_DAY || to < EDEN_TIME_SECOND || to > EDEN_TIME_DAY)
        return 0;
    from_scale = unit_seconds[from];
    to_scale = unit_seconds[to];
    if (value > UINT64_MAX / from_scale) return 0;
    seconds = value * from_scale;
    if (seconds % to_scale != 0u) return 0;
    *result = seconds / to_scale;
    return 1;
}

static int append_text(char *buffer, size_t capacity, size_t *used,
                       const char *text) {
    while (*text) {
        if (*used + 1u >= capacity) return 0;
        buffer[(*used)++] = *text++;
    }
    buffer[*used] = '\0';
    return 1;
}

static int append_ring(char *buffer, size_t capacity, size_t *used,
                       const char *label, uint8_t count, uint8_t marker_position,
                       uint8_t shown_position, char marker) {
    char prefix[32];
    int length = snprintf(prefix, sizeof prefix, "%s %02u/%02u [", label,
                          (unsigned)shown_position, (unsigned)count);
    if (length < 0 || (size_t)length >= sizeof prefix ||
        !append_text(buffer, capacity, used, prefix))
        return 0;
    for (uint8_t i = 0; i < count; ++i) {
        if (*used + 1u >= capacity) return 0;
        buffer[(*used)++] = i == marker_position ? marker : '.';
    }
    buffer[*used] = '\0';
    return append_text(buffer, capacity, used, "]\n");
}

int eden_clock_render(const EdenClockState *state, char *buffer,
                      size_t capacity) {
    char header[96];
    size_t used = 0;
    int length;
    if (!state || !buffer || capacity == 0u) return 0;
    buffer[0] = '\0';
    length = snprintf(header, sizeof header,
                      "STATE cycle=%05" PRIu32
                      " time=%02u:%02u:%02u hour_sub=%03u/720\n",
                      state->cycle_second, (unsigned)state->display_hour,
                      (unsigned)state->minute, (unsigned)state->second,
                      (unsigned)state->hour_subposition);
    if (length < 0 || (size_t)length >= sizeof header ||
        !append_text(buffer, capacity, &used, header) ||
        !append_ring(buffer, capacity, &used, "SECOND", 60u,
                     state->second_position, state->second_position, 'S') ||
        !append_ring(buffer, capacity, &used, "MINUTE", 60u,
                     state->minute_position, state->minute_position, 'M') ||
        !append_ring(buffer, capacity, &used, "HOUR  ", 12u,
                     state->hour_position, state->display_hour, 'H'))
        return 0;
    return 1;
}
