#include "shakti_internal.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static ShaktiStatus json_escape(const char *input, char *out, size_t out_size) {
    size_t used = 0u;
    size_t i;
    if (input == NULL || out == NULL || out_size == 0u) return SHAKTI_BAD_ARGUMENT;
    for (i = 0u; input[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)input[i];
        const char *escaped = NULL;
        size_t escaped_length = 0u;
        if (c == '"') { escaped = "\\\""; escaped_length = 2u; }
        else if (c == '\\') { escaped = "\\\\"; escaped_length = 2u; }
        else if (c == '\n') { escaped = "\\n"; escaped_length = 2u; }
        else if (c == '\r') { escaped = "\\r"; escaped_length = 2u; }
        else if (c == '\t') { escaped = "\\t"; escaped_length = 2u; }
        else if (c < 0x20u) return SHAKTI_REFUSED;
        if (escaped != NULL) {
            if (used + escaped_length + 1u > out_size) return SHAKTI_TOO_LARGE;
            memcpy(out + used, escaped, escaped_length);
            used += escaped_length;
        } else {
            if (used + 2u > out_size) return SHAKTI_TOO_LARGE;
            out[used++] = (char)c;
        }
    }
    out[used] = '\0';
    return SHAKTI_OK;
}

ShaktiStatus shakti_build_loopback(const ShaktiRouter *router, ShaktiLoopback *loopback) {
    char escaped_history[SHAKTI_HISTORY_MAX * 2u];
    const char *trigger;
    const char *instructions;
    int written;
    ShaktiStatus status;
    if (router == NULL || loopback == NULL) return SHAKTI_BAD_ARGUMENT;
    status = json_escape(router->short_term_history, escaped_history,
                         sizeof escaped_history);
    if (status != SHAKTI_OK) return status;
    if (router->cycle.trigger == SHAKTI_TRIGGER_HEARTBEAT) {
        trigger = "heartbeat";
        instructions = "Shakti, wake up. Is there something you wanted to do, "
                       "homework to finish, chores to handle, or music to practice? "
                       "Choose one bounded activity. When done, reflect and complete.";
    } else if (router->cycle.trigger == SHAKTI_TRIGGER_CONTINUATION) {
        trigger = "continuation";
        instructions = "Continue the unfinished task from short-term history. "
                       "Do not start a different task. Complete or request continue again.";
    } else if (router->cycle.trigger == SHAKTI_TRIGGER_REFLECTION) {
        trigger = "quiet_reflection";
        instructions = "Pause. Observe the current short-term history. Reflect on what "
                       "happened, what you may have missed, and the safest next step. "
                       "Do not call tools. Complete this reflection.";
    } else {
        return SHAKTI_BAD_STATE;
    }
    written = snprintf(loopback->json, sizeof loopback->json,
        "{\"schema_version\":1,\"task_id\":\"%" PRIu64
        "\",\"called_epoch\":%" PRIu64 ",\"trigger\":\"%s\","
        "\"operation_state\":\"new_cycle\",\"short_term_history\":\"%s\","
        "\"next_instructions\":\"%s\","
        "\"heartbeat\":{\"enabled\":%s,\"wake_remaining\":%u},"
        "\"runway\":{\"remaining\":%u},"
        "\"capabilities\":{\"tools\":\"%s\"}}",
        router->cycle.called_epoch, router->cycle.called_epoch,
        trigger, escaped_history, instructions,
        router->heartbeat.enabled ? "true" : "false",
        (unsigned)router->heartbeat.cycles_remaining,
        (unsigned)router->runway.cycles_remaining,
        router->tool_policy == SHAKTI_TOOLS_OFF ? "off" : "restricted");
    if (written < 0 || (size_t)written >= sizeof loopback->json) return SHAKTI_TOO_LARGE;
    return SHAKTI_OK;
}
