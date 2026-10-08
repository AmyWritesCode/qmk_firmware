#pragma once

#include "process_tap_dance.h"

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
} tap_dance_step_t;

tap_dance_step_t get_current_dance_step(tap_dance_state_t *state);
