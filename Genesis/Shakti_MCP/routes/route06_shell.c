#include "shakti_internal.h"

ShaktiStatus shakti_route_shell(ShaktiRouter *router, const ShaktiCommand *command,
                                ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_NOT_IMPLEMENTED,
                               "shell attachment is a transport stub and executes nothing");
}
