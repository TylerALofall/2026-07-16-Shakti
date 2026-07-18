#include "shakti_mcp.h"
#include "shakti_memory_contract.h"

#include <stdio.h>
#include <string.h>

static int expect(int condition, const char *name) {
    if (!condition) fprintf(stderr, "FAIL: %s\n", name);
    return condition;
}

static ShaktiCommand command(ShaktiRouteId route, const char *operation,
                             const char *payload) {
    ShaktiCommand value;
    memset(&value, 0, sizeof value);
    value.route = route;
    if (operation != NULL) snprintf(value.operation, sizeof value.operation, "%s", operation);
    if (payload != NULL) snprintf(value.payload, sizeof value.payload, "%s", payload);
    return value;
}

static int test_frozen_time_and_optional_message(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    ShaktiCommand epoch = command(SHAKTI_ROUTE_EPOCH, "status", "");
    ShaktiResponse response;
    shakti_router_init(&router, 40u);
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 1000u) == SHAKTI_OK, "admit Tyler cycle")) return 0;
    if (!expect(shakti_dispatch(&router, &epoch, &response) == SHAKTI_OK, "epoch route")) return 0;
    if (!expect(strstr(response.text, "called_epoch=1000") != NULL, "epoch is call time")) return 0;
    if (!expect(shakti_request_continue(&router) == SHAKTI_OK,
                "request first runway continuation")) return 0;
    if (!expect(shakti_turn_closed(&router, 5000u) == SHAKTI_OK, "close Tyler cycle")) return 0;
    if (!expect(shakti_pump(&router, 9000u, &loopback) == SHAKTI_PROMPT_READY, "first runway admitted")) return 0;
    if (!expect(router.runway.cycles_remaining == 8u, "runway spent at admission")) return 0;
    if (!expect(router.cycle.message_out_count == 0u, "message route optional")) return 0;
    if (!expect(router.cycle.called_epoch == 1000u, "runway preserves original task time")) return 0;
    if (!expect(strstr(loopback.json, "\"called_epoch\":1000") != NULL &&
                strstr(loopback.json, "\"trigger\":\"continuation\"") != NULL,
                "runway loopback keeps task identity")) return 0;
    if (!expect(shakti_dispatch(&router, &epoch, &response) == SHAKTI_OK, "epoch route during continuation")) return 0;
    if (!expect(strstr(response.text, "called_epoch=1000") != NULL, "time does not move inside task")) return 0;
    if (!expect(shakti_request_continue(&router) == SHAKTI_OK,
                "request second runway continuation")) return 0;
    if (!expect(shakti_turn_closed(&router, 15000u) == SHAKTI_OK, "close first continuation")) return 0;
    if (!expect(shakti_pump(&router, 20000u, &loopback) == SHAKTI_PROMPT_READY, "second runway admitted")) return 0;
    if (!expect(router.runway.cycles_remaining == 7u, "second runway spent without message")) return 0;
    if (!expect(shakti_turn_closed(&router, 21000u) == SHAKTI_OK, "close final continuation")) return 0;
    return expect(!router.runway.armed && router.cycle.state == SHAKTI_CYCLE_IDLE,
                  "finished task rests without a wake schedule");
}

static int test_graceful_stop(void) {
    ShaktiRouter router;
    ShaktiCheckpoint checkpoint;
    shakti_router_init(&router, 0u);
    if (!expect(shakti_heartbeat_enable(&router, 9u, 0u) == SHAKTI_OK, "enable for stop")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 111u) == SHAKTI_OK, "admit stop cycle")) return 0;
    if (!expect(shakti_request_stop(&router) == SHAKTI_OK, "request graceful stop")) return 0;
    if (!expect(router.cycle.state == SHAKTI_CYCLE_RUNNING, "active work not killed")) return 0;
    if (!expect(router.cycle.stop_requested && !router.heartbeat.enabled,
                "future continuation stopped immediately")) return 0;
    if (!expect(shakti_checkpoint_capture(&router, &checkpoint) == SHAKTI_OK,
                "capture draining checkpoint")) return 0;
    if (!expect(checkpoint.cycle.stop_requested &&
                checkpoint.cycle.state == SHAKTI_CYCLE_RUNNING,
                "checkpoint identifies unfinished active work")) return 0;
    if (!expect(shakti_turn_closed(&router, 222u) == SHAKTI_OK, "active work finishes")) return 0;
    return expect(router.cycle.state == SHAKTI_CYCLE_IDLE &&
                  !router.heartbeat.wake_armed && !router.runway.armed,
                  "drain ends idle and off");
}

static int test_stop_while_waiting(void) {
    ShaktiRouter router;
    ShaktiCheckpoint checkpoint;
    shakti_router_init(&router, 10u);
    if (!expect(shakti_heartbeat_enable(&router, 9u, 0u) == SHAKTI_OK,
                "enable waiting stop")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 700u) == SHAKTI_OK,
                "admit waiting stop")) return 0;
    if (!expect(shakti_mark_operation_pending(&router, 88u) == SHAKTI_OK,
                "pending before stop")) return 0;
    if (!expect(shakti_turn_closed(&router, 701u) == SHAKTI_WAITING,
                "wait before stop")) return 0;
    if (!expect(shakti_request_stop(&router) == SHAKTI_OK, "stop while waiting")) return 0;
    if (!expect(shakti_checkpoint_capture(&router, &checkpoint) == SHAKTI_OK,
                "capture waiting checkpoint")) return 0;
    if (!expect(checkpoint.cycle.state == SHAKTI_CYCLE_WAITING_FOR_RESULT &&
                checkpoint.cycle.pending_operation_id == 88u,
                "checkpoint names unfinished operation")) return 0;
    if (!expect(shakti_post_result(&router, 88u, SHAKTI_RESULT_FAIL, "operation failed") == SHAKTI_OK,
                "real result still returns during drain")) return 0;
    if (!expect(shakti_turn_closed(&router, 702u) == SHAKTI_OK,
                "drained waiting cycle closes")) return 0;
    return expect(!router.heartbeat.wake_armed && !router.runway.armed &&
                  !router.heartbeat.enabled,
                  "waiting drain cannot rearm");
}

static int test_result_reentry(void) {
    ShaktiRouter router;
    shakti_router_init(&router, 5u);
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 333u) == SHAKTI_OK, "admit result cycle")) return 0;
    if (!expect(shakti_mark_operation_pending(&router, 77u) == SHAKTI_OK, "mark pending operation")) return 0;
    if (!expect(shakti_turn_closed(&router, 334u) == SHAKTI_WAITING, "turn waits for result")) return 0;
    if (!expect(shakti_post_result(&router, 76u, SHAKTI_RESULT_PASS, "wrong") == SHAKTI_OPERATION_MISMATCH,
                "wrong operation refused")) return 0;
    if (!expect(shakti_post_result(&router, 77u, SHAKTI_RESULT_PASS, "done") == SHAKTI_OK,
                "result resumes same cycle")) return 0;
    if (!expect(router.cycle.called_epoch == 333u && router.cycle.cycle_sequence == 6u,
                "result cannot move task identity")) return 0;
    return expect(shakti_turn_closed(&router, 400u) == SHAKTI_OK, "resumed cycle completes");
}

static int test_event_address(void) {
    ShaktiRouter router;
    uint16_t circuit_a[1] = {1u};
    uint16_t circuit_aa[2] = {1u, 1u};
    uint16_t circuit_ab[2] = {1u, 2u};
    ShaktiEventAddress address;
    char id[SHAKTI_EVENT_ID_MAX];
    shakti_router_init(&router, 6u);
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 17939201u) == SHAKTI_OK,
                "admit address cycle")) return 0;
    if (!expect(shakti_make_event_address(&router, circuit_a, 1u, &address) == SHAKTI_OK,
                "make top circuit address")) return 0;
    if (!expect(shakti_format_event_id(&address, id, sizeof id) == SHAKTI_OK,
                "format first address")) return 0;
    if (!expect(!strcmp(id, "17939201-A-1"), "epoch circuit and first firing")) return 0;
    if (!expect(shakti_make_event_address(&router, circuit_aa, 2u, &address) == SHAKTI_OK,
                "make first subsystem address")) return 0;
    if (!expect(shakti_format_event_id(&address, id, sizeof id) == SHAKTI_OK &&
                !strcmp(id, "17939201-Aa-2"),
                "subsystem Aa and second firing")) return 0;
    if (!expect(shakti_make_event_address(&router, circuit_ab, 2u, &address) == SHAKTI_OK,
                "make second subsystem address")) return 0;
    if (!expect(shakti_format_event_id(&address, id, sizeof id) == SHAKTI_OK &&
                !strcmp(id, "17939201-Ab-3"),
                "subsystem Ab and third firing")) return 0;
    if (!expect(shakti_turn_closed(&router, 17939202u) == SHAKTI_OK,
                "finish first cycle under epoch")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 17939201u) == SHAKTI_OK,
                "second serial cycle shares frozen epoch")) return 0;
    if (!expect(shakti_make_event_address(&router, circuit_a, 1u, &address) == SHAKTI_OK,
                "address next serial firing")) return 0;
    return expect(shakti_format_event_id(&address, id, sizeof id) == SHAKTI_OK &&
                  !strcmp(id, "17939201-A-4"),
                  "same epoch continues universal firing order without cycle field");
}

static int test_switchboard_and_same_cycle_results(void) {
    ShaktiRouter router;
    ShaktiResponse response;
    ShaktiCommand value;
    ShaktiStatus expected[SHAKTI_ROUTE_COUNT] = {
        SHAKTI_OK,
        SHAKTI_OK,
        SHAKTI_NOT_IMPLEMENTED,
        SHAKTI_NOT_IMPLEMENTED,
        SHAKTI_NOT_IMPLEMENTED,
        SHAKTI_NOT_IMPLEMENTED,
        SHAKTI_OK,
        SHAKTI_REFUSED,
        SHAKTI_NOT_IMPLEMENTED
    };
    size_t i;
    shakti_router_init(&router, 0u);
    if (!expect(shakti_heartbeat_enable(&router, 3u, 0u) == SHAKTI_OK,
                "enable switchboard heartbeat")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 808u) == SHAKTI_OK,
                "admit switchboard cycle")) return 0;
    for (i = 0u; i < SHAKTI_ROUTE_COUNT; ++i) {
        value = command((ShaktiRouteId)(i + 1u), "status", "hello Tyler");
        if (!expect(shakti_dispatch(&router, &value, &response) == expected[i],
                    "each fixed route resolves")) return 0;
    }
    if (!expect(router.cycle.state == SHAKTI_CYCLE_RUNNING,
                "route results remain in same cycle")) return 0;
    if (!expect(router.heartbeat.cycles_remaining == 3u,
                "commands do not spend heartbeat budget")) return 0;
    if (!expect(router.cycle.message_out_count == 1u,
                "message out is counted only when used")) return 0;
    return expect(shakti_turn_closed(&router, 809u) == SHAKTI_OK,
                  "switchboard cycle completes once");
}

static int test_completion_anchored_delay_and_fresh_context(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    shakti_router_init(&router, 0u);
    if (!expect(shakti_heartbeat_enable(&router, 1u, 5u) == SHAKTI_OK,
                "enable delayed heartbeat")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 100u) == SHAKTI_OK,
                "admit delayed cycle")) return 0;
    if (!expect(shakti_turn_closed(&router, 200u) == SHAKTI_OK,
                "wake delay anchored at completion")) return 0;
    if (!expect(shakti_pump(&router, 204u, &loopback) == SHAKTI_OK &&
                router.cycle.state == SHAKTI_CYCLE_IDLE,
                "heartbeat cannot start before completion delay")) return 0;
    if (!expect(shakti_set_short_term_history(&router, "new \"history\"\nline") == SHAKTI_OK,
                "update context while intention armed")) return 0;
    if (!expect(shakti_pump(&router, 205u, &loopback) == SHAKTI_PROMPT_READY,
                "heartbeat wake starts after completion delay")) return 0;
    if (!expect(strstr(loopback.json, "new \\\"history\\\"\\nline") != NULL,
                "loopback is built from fresh escaped context")) return 0;
    if (!expect(strstr(loopback.json, "homework to finish") != NULL,
                "heartbeat carries the wake routine")) return 0;
    if (!expect(shakti_pump(&router, 999u, &loopback) == SHAKTI_OK,
                "active cycle blocks stacked prompt")) return 0;
    return expect(router.cycle.called_epoch == 205u,
                  "admission time remains frozen after later pumps");
}

static int test_circuit_subsystem_depth(void) {
    ShaktiRouter router;
    uint16_t circuit_path[5] = {1u, 1u, 2u, 3u, 27u};
    ShaktiEventAddress address;
    char id[SHAKTI_EVENT_ID_MAX];
    shakti_router_init(&router, 0u);
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 900u) == SHAKTI_OK,
                "admit layered event")) return 0;
    if (!expect(shakti_make_event_address(&router, circuit_path, 5u, &address) == SHAKTI_OK,
                "five useful parent levels accepted")) return 0;
    if (!expect(shakti_format_event_id(&address, id, sizeof id) == SHAKTI_OK,
                "format flexible parent depth")) return 0;
    return expect(!strcmp(id, "900-Aabcaa-1"),
                  "uppercase circuit lowercase subsystems and firing order");
}

static int test_atomic_prompt_admission_and_command_bounds(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    ShaktiCommand value;
    ShaktiResponse response;
    char large_history[4001];
    memset(large_history, 'x', sizeof large_history - 1u);
    large_history[sizeof large_history - 1u] = '\0';
    shakti_router_init(&router, 0u);
    if (!expect(shakti_heartbeat_enable(&router, 1u, 0u) == SHAKTI_OK,
                "enable atomic heartbeat")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 1000u) == SHAKTI_OK,
                "admit atomic source")) return 0;
    if (!expect(shakti_turn_closed(&router, 1001u) == SHAKTI_OK,
                "arm atomic heartbeat wake")) return 0;
    if (!expect(shakti_set_short_term_history(&router, large_history) == SHAKTI_OK,
                "accept history within history limit")) return 0;
    if (!expect(shakti_pump(&router, 1001u, &loopback) == SHAKTI_TOO_LARGE,
                "oversized bundle refused")) return 0;
    if (!expect(router.cycle.state == SHAKTI_CYCLE_IDLE &&
                router.heartbeat.cycles_remaining == 1u &&
                router.heartbeat.wake_armed,
                "failed bundle cannot spend or admit")) return 0;
    if (!expect(shakti_set_short_term_history(&router, "small") == SHAKTI_OK,
                "repair current context")) return 0;
    if (!expect(shakti_pump(&router, 1002u, &loopback) == SHAKTI_PROMPT_READY,
                "repaired bundle admits once")) return 0;
    memset(&value, 'x', sizeof value);
    value.route = SHAKTI_ROUTE_EPOCH;
    return expect(shakti_dispatch(&router, &value, &response) == SHAKTI_BAD_ARGUMENT,
                  "unterminated command refused at switchboard boundary");
}

static int test_completion_without_continue_rests(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    shakti_router_init(&router, 0u);
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 3000u) == SHAKTI_OK,
                "admit resting cycle")) return 0;
    if (!expect(shakti_turn_closed(&router, 4000u) == SHAKTI_OK,
                "complete without continue")) return 0;
    if (!expect(!router.heartbeat.wake_armed &&
                router.heartbeat.cycles_remaining == 9u,
                "completion alone cannot arm or spend")) return 0;
    return expect(shakti_pump(&router, 9000u, &loopback) == SHAKTI_OK &&
                  router.cycle.state == SHAKTI_CYCLE_IDLE,
                  "resting router stays idle");
}

static int test_memory_contract_compiles(void) {
    ShaktiReflectionCandidate candidate;
    ShaktiRecallRequest request;
    ShaktiRecallSet recalled;
    memset(&candidate, 0, sizeof candidate);
    memset(&request, 0, sizeof request);
    memset(&recalled, 0, sizeof recalled);
    candidate.evidence = SHAKTI_EVIDENCE_HYPOTHESIS;
    request.max_blocks = SHAKTI_RECALL_BLOCK_MAX;
    return expect(candidate.evidence == SHAKTI_EVIDENCE_HYPOTHESIS &&
                  request.max_blocks == 8u && recalled.block_count == 0u,
                  "memory loader and recall shapes compile without implementation");
}

static int test_host_scheduled_heartbeat_wake(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    shakti_router_init(&router, 20u);
    if (!expect(shakti_heartbeat_enable(&router, 1u, 0u) == SHAKTI_OK,
                "enable host-scheduled wake")) return 0;
    if (!expect(shakti_heartbeat_schedule(&router, 600u) == SHAKTI_OK,
                "host schedules first wake")) return 0;
    if (!expect(shakti_pump(&router, 599u, &loopback) == SHAKTI_OK,
                "scheduled wake waits for eligibility")) return 0;
    if (!expect(shakti_pump(&router, 600u, &loopback) == SHAKTI_PROMPT_READY,
                "scheduled heartbeat wakes")) return 0;
    return expect(router.cycle.trigger == SHAKTI_TRIGGER_HEARTBEAT &&
                  router.cycle.called_epoch == 600u &&
                  strstr(loopback.json, "music to practice") != NULL,
                  "heartbeat starts a new wake task with routine");
}

static int test_quiet_reflection_drains_and_closes_tools(void) {
    ShaktiRouter router;
    ShaktiLoopback loopback;
    ShaktiCommand shell = command(SHAKTI_ROUTE_SHELL, "run", "unsafe");
    ShaktiResponse response;
    shakti_router_init(&router, 30u);
    if (!expect(shakti_heartbeat_enable(&router, 9u, 0u) == SHAKTI_OK,
                "heartbeat enabled before recovery")) return 0;
    if (!expect(shakti_admit_cycle(&router, SHAKTI_TRIGGER_TYLER, 400u) == SHAKTI_OK,
                "active task before recovery")) return 0;
    if (!expect(shakti_quiet_reflection_begin(&router, 2u) == SHAKTI_OK,
                "request bounded quiet reflection")) return 0;
    if (!expect(router.tool_policy == SHAKTI_TOOLS_DRAIN &&
                router.cycle.state == SHAKTI_CYCLE_RUNNING,
                "recovery drains current work")) return 0;
    if (!expect(shakti_dispatch(&router, &shell, &response) == SHAKTI_REFUSED,
                "drain blocks new tool dispatch")) return 0;
    if (!expect(shakti_turn_closed(&router, 450u) == SHAKTI_OK,
                "active task closes before recovery")) return 0;
    if (!expect(router.mode == SHAKTI_MODE_QUIET_REFLECTION &&
                router.tool_policy == SHAKTI_TOOLS_OFF &&
                !router.heartbeat.enabled && !router.runway.armed,
                "quiet reflection starts with capabilities off")) return 0;
    if (!expect(shakti_quiet_reflection_schedule(&router, 500u) == SHAKTI_OK,
                "host schedules reflection pulse")) return 0;
    if (!expect(shakti_pump(&router, 499u, &loopback) == SHAKTI_OK,
                "reflection waits for host eligibility")) return 0;
    if (!expect(shakti_pump(&router, 500u, &loopback) == SHAKTI_PROMPT_READY,
                "reflection pulse admitted")) return 0;
    if (!expect(router.cycle.called_epoch == 500u &&
                router.cycle.trigger == SHAKTI_TRIGGER_REFLECTION &&
                strstr(loopback.json, "\"tools\":\"off\"") != NULL &&
                strstr(loopback.json, "Do not call tools") != NULL,
                "reflection gets frozen identity and no-tool envelope")) return 0;
    if (!expect(shakti_quiet_reflection_end(&router) == SHAKTI_OK,
                "host ends bounded reflection window")) return 0;
    if (!expect(shakti_turn_closed(&router, 550u) == SHAKTI_OK,
                "last reflection finishes cleanly")) return 0;
    return expect(router.mode == SHAKTI_MODE_REST &&
                  router.tool_policy == SHAKTI_TOOLS_RESTRICTED,
                  "recovery returns to rest without opening tools");
}

int main(void) {
    if (!test_frozen_time_and_optional_message()) return 1;
    if (!test_graceful_stop()) return 2;
    if (!test_result_reentry()) return 3;
    if (!test_event_address()) return 4;
    if (!test_stop_while_waiting()) return 5;
    if (!test_switchboard_and_same_cycle_results()) return 6;
    if (!test_completion_anchored_delay_and_fresh_context()) return 7;
    if (!test_circuit_subsystem_depth()) return 8;
    if (!test_atomic_prompt_admission_and_command_bounds()) return 9;
    if (!test_completion_without_continue_rests()) return 10;
    if (!test_memory_contract_compiles()) return 11;
    if (!test_host_scheduled_heartbeat_wake()) return 12;
    if (!test_quiet_reflection_drains_and_closes_tools()) return 13;
    puts("PASS: frozen task time, optional output, bounded heartbeat, drain, result re-entry");
    return 0;
}
