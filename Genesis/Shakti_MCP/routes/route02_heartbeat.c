#include "shakti_internal.h"

#include <stdio.h>
#include <string.h>

ShaktiStatus shakti_route_heartbeat(ShaktiRouter *router, const ShaktiCommand *command,
                                    ShaktiResponse *response) {
    int written;
    if (!strcmp(command->operation, "continue")) {
        ShaktiStatus status = shakti_request_continue(router);
        if (status != SHAKTI_OK)
            return shakti_response_set(response, status,
                                       "continuation unavailable or refused");
        return shakti_response_set(response, SHAKTI_OK,
                                   "one continuation requested after this cycle completes");
    }
    if (!strcmp(command->operation, "off")) {
        ShaktiStatus status = shakti_request_stop(router);
        if (status != SHAKTI_OK) return shakti_response_set(response, status, "stop refused");
        return shakti_response_set(response, SHAKTI_OK,
                                   "future heartbeat stopped; active cycle will finish");
    }
    if (command->operation[0] != '\0' && strcmp(command->operation, "status"))
        return shakti_response_set(response, SHAKTI_REFUSED,
                                   "Shakti may request continue, read status, or turn heartbeat off");
    written = snprintf(response->text, sizeof response->text,
        "enabled=%s wake_remaining=%u wake_armed=%s runway_remaining=%u "
        "continue_requested=%s runway_armed=%s stop_requested=%s",
        router->heartbeat.enabled ? "true" : "false",
        (unsigned)router->heartbeat.cycles_remaining,
        router->heartbeat.wake_armed ? "true" : "false",
        (unsigned)router->runway.cycles_remaining,
        router->cycle.continue_requested ? "true" : "false",
        router->runway.armed ? "true" : "false",
        router->cycle.stop_requested ? "true" : "false");
    if (written < 0 || (size_t)written >= sizeof response->text)
        return shakti_response_set(response, SHAKTI_TOO_LARGE, "heartbeat response too large");
    response->status = SHAKTI_OK;
    return SHAKTI_OK;
}
