#include "shakti_internal.h"

ShaktiStatus shakti_route_reflect(ShaktiRouter *router, const ShaktiCommand *command,
                                  ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_NOT_IMPLEMENTED,
                               "reflection bridge is reserved and stores nothing permanently");
}
