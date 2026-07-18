#include "shakti_internal.h"

ShaktiStatus shakti_route_notebook(ShaktiRouter *router, const ShaktiCommand *command,
                                   ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_NOT_IMPLEMENTED,
                               "notebooks and reminders are reserved for their own route pass");
}
