#include "shakti_internal.h"

ShaktiStatus shakti_route_goal(ShaktiRouter *router, const ShaktiCommand *command,
                               ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_NOT_IMPLEMENTED,
                               "goal store is reserved for its own route pass");
}
