#include "tap_dance.h"

tap_dance_step_t get_current_dance_step(tap_dance_state_t *state) {
    switch (state->count) {
        case 1:
            if (!state->pressed
#ifndef PERMISSIVE_HOLD
                || state->interrupted
#endif
            ) {
                return TD_SINGLE_TAP;
            }
            return TD_SINGLE_HOLD;
        case 2:
            return TD_DOUBLE_TAP;
        default:
            return TD_UNKNOWN;
    }
}
