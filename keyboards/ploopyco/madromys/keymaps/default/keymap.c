/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "action_layer.h"
#include "ploopyco.h"
#include "tap_dance.h"
#include QMK_KEYBOARD_H

enum layers {
    LYR_BASE,
    LYR_MEDIA,
    LYR_CONFIG,
};

/* -------- Swap hands -------- */
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 5}, {0, 0}},
};

/* -------- Tap dance -------- */
td_outcome_t td_curr_outcome(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return TD_SINGLE_TAP;
        } else {
            return TD_SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        if (state->interrupted) {
            return TD_DOUBLE_SINGLE_TAP;
        } else if (state->pressed) {
            return TD_DOUBLE_HOLD;
        } else {
            return TD_DOUBLE_TAP;
        }
    } else if (state->count == 3) {
        if (state->interrupted || !state->pressed) {
            return TD_TRIPLE_TAP;
        } else {
            return TD_TRIPLE_HOLD;
        }
    } else {
        return TD_UNKNOWN;
    }
}

static td_tap_t td_search_swaphands_config_state = {
    .is_press_action = true,
    .outcome = TD_NONE,
};

void td_search_swaphands_config_finished(tap_dance_state_t *state, void *user_data) {
    td_search_swaphands_config_state.outcome = td_curr_outcome(state);
    switch (td_search_swaphands_config_state.outcome) {
        case TD_SINGLE_TAP:
            register_code(KC_WSCH);
            break;
        case TD_DOUBLE_TAP:
            swap_hands_toggle();
            break;
        case TD_TRIPLE_TAP:
            layer_on(LYR_CONFIG);
        default:
            break;
    }
}

void td_search_swaphands_config_reset(tap_dance_state_t *state, void *user_data) {
    if (td_search_swaphands_config_state.outcome == TD_SINGLE_TAP) {
        unregister_code(KC_WSCH);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SEARCH_SWAPHANDS_CONFIG] = ACTION_TAP_DANCE_FN_ADVANCED(
        NULL,
        td_search_swaphands_config_finished,
        td_search_swaphands_config_reset
    ),
};

/* -------- Keymap --------
 *   ┌────┬───┬───┬────┐
 *   │  A │ B | C │ D  │
 *   │    ├───┴───┤    │
 *   ├────┤       ├────┤
 *   │    │       │    │
 *   │  E │       │ F  │
 *   └────┘       └────┘
*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LYR_BASE] = LAYOUT(
        MS_BTN3,        MS_BTN2,    DRAG_SCROLL,    TT(LYR_MEDIA),
        MS_BTN1,                                    TD(TD_SEARCH_SWAPHANDS_CONFIG)
    ),
    [LYR_MEDIA] = LAYOUT(
        KC_MNXT,        KC_MUTE,    PB_1,           TG(LYR_MEDIA),
        KC_MPRV,                                    KC_MPLY
    ),
    [LYR_CONFIG] = LAYOUT(
        DPI_CONFIG,     XXXXXXX,    XXXXXXX,        XXXXXXX,
        HIRES_SCROLL,                               TG(LYR_CONFIG)
    )
};

/* -------- User processing -------- */
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case LYR_MEDIA:
            set_volume_scroll(true);
            break;
        default:
            set_volume_scroll(false);
            break;
    }
    return state;
}
