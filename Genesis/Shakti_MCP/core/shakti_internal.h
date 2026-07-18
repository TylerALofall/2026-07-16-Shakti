#ifndef SHAKTI_INTERNAL_H
#define SHAKTI_INTERNAL_H

#include "shakti_mcp.h"

typedef ShaktiStatus (*ShaktiRouteHandler)(ShaktiRouter *router,
                                           const ShaktiCommand *command,
                                           ShaktiResponse *response);

ShaktiStatus shakti_build_loopback(const ShaktiRouter *router, ShaktiLoopback *loopback);
ShaktiStatus shakti_response_set(ShaktiResponse *response, ShaktiStatus status,
                                 const char *text);

ShaktiStatus shakti_route_epoch(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_heartbeat(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_goal(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_notebook(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_menu(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_shell(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_message_out(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_message_in(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);
ShaktiStatus shakti_route_reflect(ShaktiRouter *, const ShaktiCommand *, ShaktiResponse *);

#endif
