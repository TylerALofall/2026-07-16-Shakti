#ifndef EDEN_CLOCK_H
#define EDEN_CLOCK_H

#include <stddef.h>
#include <stdint.h>

enum {
    EDEN_SECONDS_PER_MINUTE = 60,
    EDEN_MINUTES_PER_HOUR = 60,
    EDEN_HOURS_PER_CYCLE = 12,
    EDEN_HOURS_PER_DAY = 24,
    EDEN_SECONDS_PER_HOUR = 3600,
    EDEN_SECONDS_PER_CYCLE = 43200,
    EDEN_SECONDS_PER_DAY = 86400,
    EDEN_CLOCK_SEMANTIC_POSITIONS = 132,
    EDEN_CLOCK_RENDER_CAPACITY = 320
};

typedef enum EdenTimeUnit {
    EDEN_TIME_SECOND = 0,
    EDEN_TIME_MINUTE = 1,
    EDEN_TIME_HOUR = 2,
    EDEN_TIME_DAY = 3
} EdenTimeUnit;

typedef struct EdenClockState {
    uint32_t cycle_second;
    uint16_t hour_subposition;
    uint8_t display_hour;
    uint8_t minute;
    uint8_t second;
    uint8_t second_position;
    uint8_t minute_position;
    uint8_t hour_position;
} EdenClockState;

_Static_assert(EDEN_SECONDS_PER_MINUTE + EDEN_MINUTES_PER_HOUR +
                   EDEN_HOURS_PER_CYCLE == EDEN_CLOCK_SEMANTIC_POSITIONS,
               "clock semantic position count drifted");
_Static_assert(EDEN_SECONDS_PER_CYCLE == 12 * 60 * 60,
               "twelve-hour state count drifted");
_Static_assert(sizeof(EdenClockState) == 12,
               "EdenClockState layout drifted");

int eden_clock_tables_valid(void);
void eden_clock_state(uint64_t total_seconds, EdenClockState *out);
int eden_time_convert_exact(uint64_t value, EdenTimeUnit from,
                            EdenTimeUnit to, uint64_t *result);
int eden_clock_render(const EdenClockState *state, char *buffer,
                      size_t capacity);

#endif
