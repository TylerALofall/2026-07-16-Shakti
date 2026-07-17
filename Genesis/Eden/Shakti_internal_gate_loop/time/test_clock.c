#include "eden_clock.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void expect_state(uint64_t input, uint32_t cycle, uint8_t hour,
                         uint8_t minute, uint8_t second, uint8_t hour_position) {
    EdenClockState state;
    eden_clock_state(input, &state);
    assert(state.cycle_second == cycle);
    assert(state.display_hour == hour);
    assert(state.minute == minute);
    assert(state.second == second);
    assert(state.second_position == second);
    assert(state.minute_position == minute);
    assert(state.hour_position == hour_position);
}

int main(void) {
    EdenClockState state;
    char visual[EDEN_CLOCK_RENDER_CAPACITY];
    uint64_t result = 0;

    assert(eden_clock_tables_valid());
    assert(EDEN_CLOCK_SEMANTIC_POSITIONS == 132);
    assert(EDEN_SECONDS_PER_CYCLE == 43200);

    expect_state(0u, 0u, 12u, 0u, 0u, 0u);
    expect_state(59u, 59u, 12u, 0u, 59u, 0u);
    expect_state(60u, 60u, 12u, 1u, 0u, 0u);
    expect_state(3599u, 3599u, 12u, 59u, 59u, 0u);
    expect_state(3600u, 3600u, 1u, 0u, 0u, 1u);
    expect_state(43199u, 43199u, 11u, 59u, 59u, 11u);
    expect_state(43200u, 0u, 12u, 0u, 0u, 0u);

    for (uint32_t cycle = 0; cycle < EDEN_SECONDS_PER_CYCLE; ++cycle) {
        uint32_t rebuilt;
        eden_clock_state(cycle, &state);
        assert(state.second_position < EDEN_SECONDS_PER_MINUTE);
        assert(state.minute_position < EDEN_MINUTES_PER_HOUR);
        assert(state.hour_position < EDEN_HOURS_PER_CYCLE);
        rebuilt = (uint32_t)state.hour_position * EDEN_SECONDS_PER_HOUR +
                  (uint32_t)state.minute_position * EDEN_SECONDS_PER_MINUTE +
                  state.second_position;
        assert(rebuilt == cycle);
    }

    eden_clock_state(43199u, &state);
    assert(state.hour_subposition == 719u);

    assert(eden_time_convert_exact(1u, EDEN_TIME_MINUTE,
                                   EDEN_TIME_SECOND, &result));
    assert(result == 60u);
    assert(eden_time_convert_exact(2u, EDEN_TIME_HOUR,
                                   EDEN_TIME_MINUTE, &result));
    assert(result == 120u);
    assert(eden_time_convert_exact(1u, EDEN_TIME_DAY,
                                   EDEN_TIME_SECOND, &result));
    assert(result == 86400u);
    assert(!eden_time_convert_exact(1u, EDEN_TIME_SECOND,
                                    EDEN_TIME_MINUTE, &result));

    eden_clock_state(60u, &state);
    assert(eden_clock_render(&state, visual, sizeof visual));
    assert(strstr(visual, "time=12:01:00") != NULL);
    assert(strstr(visual, "hour_sub=001/720") != NULL);
    assert(strstr(visual, "SECOND 00/60 [S") != NULL);
    assert(strstr(visual, "MINUTE 01/60 [.M") != NULL);
    assert(strstr(visual, "HOUR   12/12 [H") != NULL);
    assert(!eden_clock_render(&state, visual, 16u));

    puts("PASS clock states=43200 semantic_positions=132 conversions=exact");
    return 0;
}
