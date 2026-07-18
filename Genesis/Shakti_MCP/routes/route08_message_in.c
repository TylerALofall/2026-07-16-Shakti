#include "shakti_internal.h"

ShaktiStatus shakti_route_message_in(ShaktiRouter *router, const ShaktiCommand *command,
                                     ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_REFUSED,
                               "message.in belongs to the trusted host, not Shakti");
}
