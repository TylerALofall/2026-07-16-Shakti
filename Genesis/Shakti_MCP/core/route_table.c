#include "shakti_internal.h"

#include <string.h>

static const ShaktiRouteHandler ROUTES[SHAKTI_ROUTE_COUNT] = {
    shakti_route_epoch,
    shakti_route_heartbeat,
    shakti_route_goal,
    shakti_route_notebook,
    shakti_route_menu,
    shakti_route_shell,
    shakti_route_message_out,
    shakti_route_message_in,
    shakti_route_reflect
};

_Static_assert(sizeof ROUTES / sizeof ROUTES[0] == SHAKTI_ROUTE_COUNT,
               "the switchboard must contain exactly nine routes");

ShaktiStatus shakti_response_set(ShaktiResponse *response, ShaktiStatus status,
                                 const char *text) {
    size_t length;
    if (response == NULL || text == NULL) return SHAKTI_BAD_ARGUMENT;
    length = strlen(text);
    if (length >= sizeof response->text) return SHAKTI_TOO_LARGE;
    response->status = status;
    memcpy(response->text, text, length + 1u);
    return status;
}

ShaktiStatus shakti_dispatch(ShaktiRouter *router, const ShaktiCommand *command,
                             ShaktiResponse *response) {
    size_t index;
    if (router == NULL || command == NULL || response == NULL)
        return SHAKTI_BAD_ARGUMENT;
    response->text[0] = '\0';
    response->status = SHAKTI_BAD_STATE;
    if (router->cycle.state != SHAKTI_CYCLE_RUNNING) return SHAKTI_BAD_STATE;
    if (memchr(command->operation, '\0', sizeof command->operation) == NULL ||
        memchr(command->payload, '\0', sizeof command->payload) == NULL)
        return shakti_response_set(response, SHAKTI_BAD_ARGUMENT,
                                   "command field is not terminated");
    if (command->route < SHAKTI_ROUTE_EPOCH || command->route > SHAKTI_ROUTE_REFLECT)
        return shakti_response_set(response, SHAKTI_BAD_ROUTE, "unknown route");
    if ((router->tool_policy == SHAKTI_TOOLS_OFF ||
         router->tool_policy == SHAKTI_TOOLS_DRAIN) &&
        command->route != SHAKTI_ROUTE_EPOCH &&
        command->route != SHAKTI_ROUTE_HEARTBEAT &&
        command->route != SHAKTI_ROUTE_REFLECT)
        return shakti_response_set(response, SHAKTI_REFUSED,
                                   "capability gate closed; tool request was not dispatched");
    index = (size_t)command->route - 1u;
    return ROUTES[index](router, command, response);
}
