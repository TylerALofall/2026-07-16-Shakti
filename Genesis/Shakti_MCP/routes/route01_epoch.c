#include "shakti_internal.h"

#include <inttypes.h>
#include <stdio.h>

ShaktiStatus shakti_route_epoch(ShaktiRouter *router, const ShaktiCommand *command,
                                ShaktiResponse *response) {
    int written;
    (void)command;
    written = snprintf(response->text, sizeof response->text,
        "called_epoch=%" PRIu64 " cycle=%" PRIu64 " state=%s heartbeat=%s "
        "wake_remaining=%u runway_remaining=%u",
        router->cycle.called_epoch, router->cycle.cycle_sequence,
        shakti_cycle_state_name(router->cycle.state),
        router->heartbeat.enabled ? "ON" : "OFF",
        (unsigned)router->heartbeat.cycles_remaining,
        (unsigned)router->runway.cycles_remaining);
    if (written < 0 || (size_t)written >= sizeof response->text)
        return shakti_response_set(response, SHAKTI_TOO_LARGE, "epoch response too large");
    response->status = SHAKTI_OK;
    return SHAKTI_OK;
}
