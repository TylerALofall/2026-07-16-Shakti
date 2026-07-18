#include "shakti_internal.h"

ShaktiStatus shakti_route_menu(ShaktiRouter *router, const ShaktiCommand *command,
                               ShaktiResponse *response) {
    (void)router;
    (void)command;
    return shakti_response_set(response, SHAKTI_NOT_IMPLEMENTED,
                               "menu schema exists at config/tool_menu.xml");
}
