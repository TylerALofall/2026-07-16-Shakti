#include "shakti_internal.h"

#include <stdint.h>
#include <string.h>

static void mark_transition(ShaktiRouter *router) {
    ++router->transition_sequence;
}

void shakti_router_init(ShaktiRouter *router, uint64_t persisted_cycle_sequence) {
    if (router == NULL) return;
    memset(router, 0, sizeof *router);
    router->cycle.state = SHAKTI_CYCLE_IDLE;
    router->global_cycle_sequence = persisted_cycle_sequence;
    router->heartbeat.cycles_remaining = SHAKTI_HEARTBEAT_BUDGET_DEFAULT;
    router->runway.cycles_remaining = SHAKTI_RUNWAY_BUDGET_DEFAULT;
    router->mode = SHAKTI_MODE_REST;
    router->tool_policy = SHAKTI_TOOLS_RESTRICTED;
    /* Heartbeat always boots off. A host must enable it deliberately. */
    router->heartbeat.enabled = 0;
}

ShaktiStatus shakti_set_short_term_history(ShaktiRouter *router, const char *history) {
    size_t length;
    if (router == NULL || history == NULL) return SHAKTI_BAD_ARGUMENT;
    length = strlen(history);
    if (length >= sizeof router->short_term_history) return SHAKTI_TOO_LARGE;
    memcpy(router->short_term_history, history, length + 1u);
    ++router->context_version;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_heartbeat_enable(ShaktiRouter *router, uint8_t budget,
                                     uint32_t delay_after_completion) {
    if (router == NULL || budget == 0u) return SHAKTI_BAD_ARGUMENT;
    router->heartbeat.enabled = 1;
    router->heartbeat.cycles_remaining = budget;
    router->heartbeat.delay_after_completion = delay_after_completion;
    router->heartbeat.wake_armed = 0;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_heartbeat_schedule(ShaktiRouter *router,
                                       uint64_t eligible_epoch) {
    if (router == NULL || eligible_epoch == 0u) return SHAKTI_BAD_ARGUMENT;
    if (!router->heartbeat.enabled || router->heartbeat.cycles_remaining == 0u)
        return SHAKTI_REFUSED;
    router->heartbeat.eligible_after_epoch = eligible_epoch;
    router->heartbeat.source_cycle_sequence = router->cycle.cycle_sequence;
    router->heartbeat.source_context_version = router->context_version;
    router->heartbeat.wake_armed = 1;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_request_continue(ShaktiRouter *router) {
    if (router == NULL) return SHAKTI_BAD_ARGUMENT;
    if (router->cycle.state != SHAKTI_CYCLE_RUNNING ||
        router->cycle.stop_requested || router->runway.cycles_remaining == 0u)
        return SHAKTI_REFUSED;
    router->cycle.continue_requested = 1;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_request_stop(ShaktiRouter *router) {
    if (router == NULL) return SHAKTI_BAD_ARGUMENT;
    router->heartbeat.enabled = 0;
    router->heartbeat.wake_armed = 0;
    router->runway.armed = 0;
    router->quiet_reflection.requested = 0;
    router->quiet_reflection.enabled = 0;
    router->quiet_reflection.pulse_armed = 0;
    router->mode = SHAKTI_MODE_OFF;
    router->tool_policy = SHAKTI_TOOLS_DRAIN;
    router->cycle.continue_requested = 0;
    if (router->cycle.state == SHAKTI_CYCLE_IDLE) {
        router->cycle.stop_requested = 0;
    } else {
        /* The active cycle drains normally; only future admission stops. */
        router->cycle.stop_requested = 1;
    }
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_quiet_reflection_begin(ShaktiRouter *router,
                                           uint8_t pulse_budget) {
    if (router == NULL || pulse_budget == 0u) return SHAKTI_BAD_ARGUMENT;
    router->heartbeat.enabled = 0;
    router->heartbeat.wake_armed = 0;
    router->runway.armed = 0;
    router->cycle.continue_requested = 0;
    router->quiet_reflection.requested = 1;
    router->quiet_reflection.enabled = 0;
    router->quiet_reflection.pulses_remaining = pulse_budget;
    router->quiet_reflection.pulse_armed = 0;
    router->tool_policy = SHAKTI_TOOLS_DRAIN;
    if (router->cycle.state == SHAKTI_CYCLE_IDLE) {
        router->quiet_reflection.requested = 0;
        router->quiet_reflection.enabled = 1;
        router->mode = SHAKTI_MODE_QUIET_REFLECTION;
        router->tool_policy = SHAKTI_TOOLS_OFF;
    }
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_quiet_reflection_schedule(ShaktiRouter *router,
                                              uint64_t eligible_epoch) {
    if (router == NULL || eligible_epoch == 0u) return SHAKTI_BAD_ARGUMENT;
    if (!router->quiet_reflection.enabled ||
        router->mode != SHAKTI_MODE_QUIET_REFLECTION ||
        router->quiet_reflection.pulses_remaining == 0u)
        return SHAKTI_REFUSED;
    router->quiet_reflection.eligible_after_epoch = eligible_epoch;
    router->quiet_reflection.pulse_armed = 1;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_quiet_reflection_end(ShaktiRouter *router) {
    if (router == NULL) return SHAKTI_BAD_ARGUMENT;
    router->quiet_reflection.requested = 0;
    router->quiet_reflection.enabled = 0;
    router->quiet_reflection.pulse_armed = 0;
    router->quiet_reflection.pulses_remaining = 0u;
    if (router->cycle.state == SHAKTI_CYCLE_IDLE) {
        router->mode = SHAKTI_MODE_REST;
        router->tool_policy = SHAKTI_TOOLS_RESTRICTED;
    }
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_admit_cycle(ShaktiRouter *router, ShaktiTrigger trigger,
                                uint64_t called_epoch) {
    if (router == NULL || called_epoch == 0u) return SHAKTI_BAD_ARGUMENT;
    if (router->cycle.state != SHAKTI_CYCLE_IDLE) return SHAKTI_BAD_STATE;
    if (router->global_cycle_sequence == UINT64_MAX) return SHAKTI_REFUSED;
    if (router->mode == SHAKTI_MODE_QUIET_REFLECTION &&
        trigger != SHAKTI_TRIGGER_REFLECTION) return SHAKTI_REFUSED;
    if (trigger == SHAKTI_TRIGGER_REFLECTION) {
        if (!router->quiet_reflection.enabled ||
            !router->quiet_reflection.pulse_armed ||
            router->quiet_reflection.pulses_remaining == 0u)
            return SHAKTI_REFUSED;
        router->quiet_reflection.pulse_armed = 0;
        --router->quiet_reflection.pulses_remaining;
    } else if (trigger == SHAKTI_TRIGGER_HEARTBEAT) {
        if (!router->heartbeat.enabled || !router->heartbeat.wake_armed ||
            router->heartbeat.cycles_remaining == 0u) return SHAKTI_REFUSED;
        router->heartbeat.wake_armed = 0;
        --router->heartbeat.cycles_remaining;
        router->runway.cycles_remaining = SHAKTI_RUNWAY_BUDGET_DEFAULT;
        router->runway.armed = 0;
    } else if (trigger == SHAKTI_TRIGGER_CONTINUATION) {
        if (!router->runway.armed || router->runway.cycles_remaining == 0u ||
            called_epoch != router->runway.source_called_epoch)
            return SHAKTI_REFUSED;
        router->runway.armed = 0;
        --router->runway.cycles_remaining;
    } else {
        router->runway.cycles_remaining = SHAKTI_RUNWAY_BUDGET_DEFAULT;
        router->runway.armed = 0;
    }
    ++router->global_cycle_sequence;
    if (router->event_epoch != called_epoch) {
        router->event_epoch = called_epoch;
        router->event_firing_sequence = 0u;
    }
    memset(&router->cycle, 0, sizeof router->cycle);
    router->cycle.state = SHAKTI_CYCLE_RUNNING;
    router->cycle.trigger = trigger;
    router->cycle.called_epoch = called_epoch;
    router->cycle.cycle_sequence = router->global_cycle_sequence;
    if (trigger == SHAKTI_TRIGGER_HEARTBEAT) router->mode = SHAKTI_MODE_CRUISE;
    else if (trigger == SHAKTI_TRIGGER_REFLECTION)
        router->mode = SHAKTI_MODE_QUIET_REFLECTION;
    else if (trigger != SHAKTI_TRIGGER_CONTINUATION) router->mode = SHAKTI_MODE_FOCUS;
    router->last_result = SHAKTI_RESULT_NONE;
    router->last_result_payload[0] = '\0';
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_mark_operation_pending(ShaktiRouter *router, uint64_t operation_id) {
    if (router == NULL || operation_id == 0u) return SHAKTI_BAD_ARGUMENT;
    if (router->cycle.state != SHAKTI_CYCLE_RUNNING ||
        router->cycle.pending_operation_id != 0u) return SHAKTI_BAD_STATE;
    router->cycle.pending_operation_id = operation_id;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_post_result(ShaktiRouter *router, uint64_t operation_id,
                                ShaktiResult result, const char *payload) {
    size_t length;
    if (router == NULL || payload == NULL || result == SHAKTI_RESULT_NONE)
        return SHAKTI_BAD_ARGUMENT;
    if (router->cycle.state != SHAKTI_CYCLE_WAITING_FOR_RESULT ||
        router->cycle.pending_operation_id != operation_id)
        return SHAKTI_OPERATION_MISMATCH;
    length = strlen(payload);
    if (length >= sizeof router->last_result_payload) return SHAKTI_TOO_LARGE;
    memcpy(router->last_result_payload, payload, length + 1u);
    router->last_result = result;
    router->cycle.pending_operation_id = 0u;
    router->cycle.state = SHAKTI_CYCLE_RUNNING;
    ++router->context_version;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_turn_closed(ShaktiRouter *router, uint64_t completed_epoch) {
    if (router == NULL || completed_epoch == 0u) return SHAKTI_BAD_ARGUMENT;
    if (router->cycle.state != SHAKTI_CYCLE_RUNNING) return SHAKTI_BAD_STATE;
    if (router->cycle.pending_operation_id != 0u) {
        router->cycle.state = SHAKTI_CYCLE_WAITING_FOR_RESULT;
        mark_transition(router);
        return SHAKTI_WAITING;
    }
    ++router->context_version;
    if (router->quiet_reflection.requested) {
        router->quiet_reflection.requested = 0;
        router->quiet_reflection.enabled = 1;
        router->quiet_reflection.pulse_armed = 0;
        router->mode = SHAKTI_MODE_QUIET_REFLECTION;
        router->tool_policy = SHAKTI_TOOLS_OFF;
        router->heartbeat.wake_armed = 0;
        router->runway.armed = 0;
    } else if (router->cycle.trigger == SHAKTI_TRIGGER_REFLECTION) {
        router->heartbeat.wake_armed = 0;
        router->runway.armed = 0;
        if (!router->quiet_reflection.enabled) {
            router->mode = SHAKTI_MODE_REST;
            router->tool_policy = SHAKTI_TOOLS_RESTRICTED;
        }
    } else if (router->cycle.continue_requested && !router->cycle.stop_requested &&
        router->runway.cycles_remaining > 0u) {
        router->runway.source_called_epoch = router->cycle.called_epoch;
        router->runway.source_cycle_sequence = router->cycle.cycle_sequence;
        router->runway.armed = 1;
        router->heartbeat.wake_armed = 0;
    } else if (!router->cycle.stop_requested && router->heartbeat.enabled &&
               router->heartbeat.cycles_remaining > 0u) {
        if (completed_epoch > UINT64_MAX - router->heartbeat.delay_after_completion)
            return SHAKTI_REFUSED;
        router->heartbeat.eligible_after_epoch =
            completed_epoch + router->heartbeat.delay_after_completion;
        router->heartbeat.source_cycle_sequence = router->cycle.cycle_sequence;
        router->heartbeat.source_context_version = router->context_version;
        router->heartbeat.wake_armed = 1;
        router->runway.armed = 0;
    } else {
        router->heartbeat.wake_armed = 0;
        router->runway.armed = 0;
        if (router->heartbeat.cycles_remaining == 0u) router->heartbeat.enabled = 0;
        if (router->cycle.stop_requested) {
            router->mode = SHAKTI_MODE_OFF;
            router->tool_policy = SHAKTI_TOOLS_OFF;
        }
    }
    router->cycle.state = SHAKTI_CYCLE_IDLE;
    router->cycle.continue_requested = 0;
    router->cycle.stop_requested = 0;
    mark_transition(router);
    return SHAKTI_OK;
}

ShaktiStatus shakti_checkpoint_capture(const ShaktiRouter *router,
                                       ShaktiCheckpoint *checkpoint) {
    if (router == NULL || checkpoint == NULL) return SHAKTI_BAD_ARGUMENT;
    memset(checkpoint, 0, sizeof *checkpoint);
    checkpoint->schema_version = 1u;
    checkpoint->transition_sequence = router->transition_sequence;
    checkpoint->global_cycle_sequence = router->global_cycle_sequence;
    checkpoint->event_epoch = router->event_epoch;
    checkpoint->event_firing_sequence = router->event_firing_sequence;
    checkpoint->context_version = router->context_version;
    checkpoint->cycle = router->cycle;
    checkpoint->heartbeat = router->heartbeat;
    checkpoint->runway = router->runway;
    checkpoint->quiet_reflection = router->quiet_reflection;
    checkpoint->mode = router->mode;
    checkpoint->tool_policy = router->tool_policy;
    return SHAKTI_OK;
}

ShaktiStatus shakti_pump(ShaktiRouter *router, uint64_t now_epoch,
                         ShaktiLoopback *loopback) {
    ShaktiRouter candidate;
    ShaktiLoopback candidate_loopback;
    ShaktiStatus status;
    if (router == NULL || loopback == NULL || now_epoch == 0u)
        return SHAKTI_BAD_ARGUMENT;
    loopback->json[0] = '\0';
    if (router->cycle.state != SHAKTI_CYCLE_IDLE) return SHAKTI_OK;
    /* Build against a fixed candidate so failure cannot spend or admit a cycle. */
    candidate = *router;
    if (candidate.quiet_reflection.enabled &&
        candidate.quiet_reflection.pulse_armed) {
        if (now_epoch < candidate.quiet_reflection.eligible_after_epoch)
            return SHAKTI_OK;
        status = shakti_admit_cycle(&candidate, SHAKTI_TRIGGER_REFLECTION,
                                    now_epoch);
    } else if (candidate.runway.armed) {
        status = shakti_admit_cycle(&candidate, SHAKTI_TRIGGER_CONTINUATION,
                                    candidate.runway.source_called_epoch);
    } else {
        if (!candidate.heartbeat.enabled || !candidate.heartbeat.wake_armed ||
            now_epoch < candidate.heartbeat.eligible_after_epoch) return SHAKTI_OK;
        status = shakti_admit_cycle(&candidate, SHAKTI_TRIGGER_HEARTBEAT, now_epoch);
    }
    if (status != SHAKTI_OK) return status;
    status = shakti_build_loopback(&candidate, &candidate_loopback);
    if (status != SHAKTI_OK) return status;
    *router = candidate;
    *loopback = candidate_loopback;
    return SHAKTI_PROMPT_READY;
}

const char *shakti_status_name(ShaktiStatus status) {
    switch (status) {
        case SHAKTI_OK: return "OK";
        case SHAKTI_WAITING: return "WAITING";
        case SHAKTI_PROMPT_READY: return "PROMPT_READY";
        case SHAKTI_REFUSED: return "REFUSED";
        case SHAKTI_BAD_ARGUMENT: return "BAD_ARGUMENT";
        case SHAKTI_BAD_STATE: return "BAD_STATE";
        case SHAKTI_BAD_ROUTE: return "BAD_ROUTE";
        case SHAKTI_TOO_LARGE: return "TOO_LARGE";
        case SHAKTI_NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case SHAKTI_OPERATION_MISMATCH: return "OPERATION_MISMATCH";
        default: return "UNKNOWN_STATUS";
    }
}

const char *shakti_cycle_state_name(ShaktiCycleState state) {
    switch (state) {
        case SHAKTI_CYCLE_IDLE: return "IDLE";
        case SHAKTI_CYCLE_RUNNING: return "RUNNING";
        case SHAKTI_CYCLE_WAITING_FOR_RESULT: return "WAITING_FOR_RESULT";
        default: return "UNKNOWN_STATE";
    }
}
