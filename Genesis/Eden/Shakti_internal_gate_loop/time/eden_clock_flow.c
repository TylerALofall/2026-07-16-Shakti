#define _POSIX_C_SOURCE 200809L

#include "eden_clock.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

typedef struct FlowOptions {
    uint32_t seconds_ms;
    uint32_t minutes_ms;
    int wait_enabled;
    int animate;
    int run_seconds;
    int run_minutes;
} FlowOptions;

static uint64_t monotonic_ms(void) {
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) return 0;
    return (uint64_t)now.tv_sec * 1000u + (uint64_t)now.tv_nsec / 1000000u;
#endif
}

static void sleep_ms(uint32_t milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    struct timespec request;
    struct timespec remainder;
    request.tv_sec = (time_t)(milliseconds / 1000u);
    request.tv_nsec = (long)(milliseconds % 1000u) * 1000000L;
    while (nanosleep(&request, &remainder) != 0 && errno == EINTR)
        request = remainder;
#endif
}

static void wait_until(uint64_t deadline) {
    for (;;) {
        uint64_t now = monotonic_ms();
        uint64_t remaining;
        if (now >= deadline) return;
        remaining = deadline - now;
        sleep_ms((uint32_t)(remaining > 1000u ? 1000u : remaining));
    }
}

static int parse_ms(const char *text, uint32_t *value) {
    char *end = NULL;
    unsigned long parsed;
    if (!text || !*text || !value) return 0;
    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno || !end || *end || parsed > 60000ul) return 0;
    *value = (uint32_t)parsed;
    return 1;
}

static void usage(const char *program) {
    fprintf(stderr,
            "usage: %s [--seconds-ms 0..60000] [--minutes-ms 0..60000]\n"
            "          [--no-wait] [--animate] [--seconds-only|--minutes-only]\n",
            program);
}

static int options_parse(int argc, char **argv, FlowOptions *options) {
    options->seconds_ms = 1000u;
    options->minutes_ms = 100u;
    options->wait_enabled = 1;
    options->animate = 0;
    options->run_seconds = 1;
    options->run_minutes = 1;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--no-wait")) options->wait_enabled = 0;
        else if (!strcmp(argv[i], "--animate")) options->animate = 1;
        else if (!strcmp(argv[i], "--seconds-only")) options->run_minutes = 0;
        else if (!strcmp(argv[i], "--minutes-only")) options->run_seconds = 0;
        else if (!strcmp(argv[i], "--seconds-ms") && i + 1 < argc) {
            if (!parse_ms(argv[++i], &options->seconds_ms)) return 0;
        } else if (!strcmp(argv[i], "--minutes-ms") && i + 1 < argc) {
            if (!parse_ms(argv[++i], &options->minutes_ms)) return 0;
        } else return 0;
    }
    return options->run_seconds || options->run_minutes;
}

static int run_phase(const char *name, uint32_t seconds_per_step,
                     uint32_t interval_ms, const FlowOptions *options) {
    uint64_t origin = monotonic_ms();
    for (uint32_t step = 1; step <= 60u; ++step) {
        EdenClockState state;
        char visual[EDEN_CLOCK_RENDER_CAPACITY];
        if (options->wait_enabled)
            wait_until(origin + (uint64_t)step * interval_ms);
        eden_clock_state((uint64_t)step * seconds_per_step, &state);
        if (!eden_clock_render(&state, visual, sizeof visual)) return 0;
        if (options->animate) fputs("\033[H\033[J", stdout);
        printf("PHASE %s step=%02u/60 caller_interval_ms=%u\n%s",
               name, (unsigned)step, (unsigned)interval_ms, visual);
        fflush(stdout);
    }
    return 1;
}

int main(int argc, char **argv) {
    FlowOptions options;
    if (!options_parse(argc, argv, &options) || !eden_clock_tables_valid()) {
        usage(argv[0]);
        return 2;
    }
    puts("CLOCK LAW positions=60_seconds+60_minutes+12_hours=132 states=43200");
    if (options.run_seconds &&
        !run_phase("SECONDS_REAL", 1u, options.seconds_ms, &options))
        return 1;
    if (options.run_minutes &&
        !run_phase("MINUTES_VISUAL", 60u, options.minutes_ms, &options))
        return 1;
    return 0;
}
