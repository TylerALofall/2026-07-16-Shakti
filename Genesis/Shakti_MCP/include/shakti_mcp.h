#ifndef SHAKTI_MCP_H
#define SHAKTI_MCP_H

#include <stddef.h>
#include <stdint.h>

#define SHAKTI_COMMAND_NAME_MAX 32u
#define SHAKTI_MESSAGE_MAX 2048u
#define SHAKTI_RESPONSE_MAX 4096u
#define SHAKTI_HISTORY_MAX 4096u
#define SHAKTI_CIRCUIT_DEPTH_MAX 8u
#define SHAKTI_EVENT_ID_MAX 128u
#define SHAKTI_HEARTBEAT_BUDGET_DEFAULT 9u
#define SHAKTI_RUNWAY_BUDGET_DEFAULT 9u

typedef enum {
    SHAKTI_OK = 0,
    SHAKTI_WAITING,
    SHAKTI_PROMPT_READY,
    SHAKTI_REFUSED,
    SHAKTI_BAD_ARGUMENT,
    SHAKTI_BAD_STATE,
    SHAKTI_BAD_ROUTE,
    SHAKTI_TOO_LARGE,
    SHAKTI_NOT_IMPLEMENTED,
    SHAKTI_OPERATION_MISMATCH
} ShaktiStatus;

typedef enum {
    SHAKTI_CYCLE_IDLE = 0,
    SHAKTI_CYCLE_RUNNING,
    SHAKTI_CYCLE_WAITING_FOR_RESULT
} ShaktiCycleState;

typedef enum {
    SHAKTI_TRIGGER_TYLER = 0,
    SHAKTI_TRIGGER_REMINDER,
    SHAKTI_TRIGGER_HEARTBEAT,
    SHAKTI_TRIGGER_CONTINUATION,
    SHAKTI_TRIGGER_REFLECTION
} ShaktiTrigger;

typedef enum {
    SHAKTI_MODE_REST = 0,
    SHAKTI_MODE_CRUISE,
    SHAKTI_MODE_FOCUS,
    SHAKTI_MODE_QUIET_REFLECTION,
    SHAKTI_MODE_OFF
} ShaktiMode;

typedef enum {
    SHAKTI_TOOLS_OPEN = 0,
    SHAKTI_TOOLS_RESTRICTED,
    SHAKTI_TOOLS_DRAIN,
    SHAKTI_TOOLS_OFF
} ShaktiToolPolicy;

typedef enum {
    SHAKTI_RESULT_NONE = 0,
    SHAKTI_RESULT_PASS,
    SHAKTI_RESULT_FAIL
} ShaktiResult;

typedef enum {
    SHAKTI_ROUTE_EPOCH = 1,
    SHAKTI_ROUTE_HEARTBEAT,
    SHAKTI_ROUTE_GOAL,
    SHAKTI_ROUTE_NOTEBOOK,
    SHAKTI_ROUTE_MENU,
    SHAKTI_ROUTE_SHELL,
    SHAKTI_ROUTE_MESSAGE_OUT,
    SHAKTI_ROUTE_MESSAGE_IN,
    SHAKTI_ROUTE_REFLECT,
    SHAKTI_ROUTE_COUNT = 9
} ShaktiRouteId;

typedef struct {
    uint64_t called_epoch;
    uint16_t circuit_path[SHAKTI_CIRCUIT_DEPTH_MAX];
    uint8_t circuit_depth;
    uint32_t firing_order;
} ShaktiEventAddress;

typedef struct {
    ShaktiRouteId route;
    char operation[SHAKTI_COMMAND_NAME_MAX];
    char payload[SHAKTI_MESSAGE_MAX];
} ShaktiCommand;

typedef struct {
    ShaktiStatus status;
    char text[SHAKTI_RESPONSE_MAX];
} ShaktiResponse;

typedef struct {
    char json[SHAKTI_RESPONSE_MAX];
} ShaktiLoopback;

typedef struct {
    int enabled;
    uint8_t cycles_remaining;
    uint32_t delay_after_completion;
    uint64_t eligible_after_epoch;
    uint64_t source_cycle_sequence;
    uint64_t source_context_version;
    int wake_armed;
} ShaktiHeartbeatState;

typedef struct {
    uint8_t cycles_remaining;
    uint64_t source_called_epoch;
    uint64_t source_cycle_sequence;
    int armed;
} ShaktiRunwayState;

typedef struct {
    int requested;
    int enabled;
    uint8_t pulses_remaining;
    uint64_t eligible_after_epoch;
    int pulse_armed;
} ShaktiQuietReflectionState;

typedef struct {
    ShaktiCycleState state;
    ShaktiTrigger trigger;
    uint64_t called_epoch;
    uint64_t cycle_sequence;
    uint64_t pending_operation_id;
    uint32_t message_out_count;
    int continue_requested;
    int stop_requested;
} ShaktiCycle;

typedef struct ShaktiRouter {
    ShaktiCycle cycle;
    ShaktiHeartbeatState heartbeat;
    ShaktiRunwayState runway;
    ShaktiQuietReflectionState quiet_reflection;
    ShaktiMode mode;
    ShaktiToolPolicy tool_policy;
    uint64_t global_cycle_sequence;
    uint64_t event_epoch;
    uint32_t event_firing_sequence;
    uint64_t context_version;
    uint64_t transition_sequence;
    ShaktiResult last_result;
    char last_result_payload[SHAKTI_MESSAGE_MAX];
    char short_term_history[SHAKTI_HISTORY_MAX];
} ShaktiRouter;

typedef struct {
    uint32_t schema_version;
    uint64_t transition_sequence;
    uint64_t global_cycle_sequence;
    uint64_t event_epoch;
    uint32_t event_firing_sequence;
    uint64_t context_version;
    ShaktiCycle cycle;
    ShaktiHeartbeatState heartbeat;
    ShaktiRunwayState runway;
    ShaktiQuietReflectionState quiet_reflection;
    ShaktiMode mode;
    ShaktiToolPolicy tool_policy;
} ShaktiCheckpoint;

void shakti_router_init(ShaktiRouter *router, uint64_t persisted_cycle_sequence);
ShaktiStatus shakti_set_short_term_history(ShaktiRouter *router, const char *history);
ShaktiStatus shakti_heartbeat_enable(ShaktiRouter *router, uint8_t budget,
                                     uint32_t delay_after_completion);
ShaktiStatus shakti_heartbeat_schedule(ShaktiRouter *router,
                                       uint64_t eligible_epoch);
ShaktiStatus shakti_request_continue(ShaktiRouter *router);
ShaktiStatus shakti_request_stop(ShaktiRouter *router);
ShaktiStatus shakti_quiet_reflection_begin(ShaktiRouter *router,
                                           uint8_t pulse_budget);
ShaktiStatus shakti_quiet_reflection_schedule(ShaktiRouter *router,
                                              uint64_t eligible_epoch);
ShaktiStatus shakti_quiet_reflection_end(ShaktiRouter *router);
ShaktiStatus shakti_admit_cycle(ShaktiRouter *router, ShaktiTrigger trigger,
                                uint64_t called_epoch);
ShaktiStatus shakti_turn_closed(ShaktiRouter *router, uint64_t completed_epoch);
ShaktiStatus shakti_mark_operation_pending(ShaktiRouter *router, uint64_t operation_id);
ShaktiStatus shakti_post_result(ShaktiRouter *router, uint64_t operation_id,
                                ShaktiResult result, const char *payload);
ShaktiStatus shakti_pump(ShaktiRouter *router, uint64_t now_epoch,
                         ShaktiLoopback *loopback);
ShaktiStatus shakti_dispatch(ShaktiRouter *router, const ShaktiCommand *command,
                             ShaktiResponse *response);
ShaktiStatus shakti_make_event_address(ShaktiRouter *router,
                                       const uint16_t *circuit_path,
                                       uint8_t circuit_depth,
                                       ShaktiEventAddress *address);
ShaktiStatus shakti_format_event_id(const ShaktiEventAddress *address,
                                    char *out, size_t out_size);
ShaktiStatus shakti_checkpoint_capture(const ShaktiRouter *router,
                                       ShaktiCheckpoint *checkpoint);
const char *shakti_status_name(ShaktiStatus status);
const char *shakti_cycle_state_name(ShaktiCycleState state);

#endif
