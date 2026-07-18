#include "shakti_internal.h"

ShaktiStatus shakti_route_message_out(ShaktiRouter *router, const ShaktiCommand *command,
                                      ShaktiResponse *response) {
    ++router->cycle.message_out_count;
    ++router->context_version;
    ++router->transition_sequence;
    return shakti_response_set(response, SHAKTI_OK, command->payload);
}
