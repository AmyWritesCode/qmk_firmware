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
#include "process_tap_dance.h"
#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _MEDIA,
    _CONFIG,
};

enum tap_dances {
    TD_SEARCH_SWAPHANDS,
    TD_MEDIA_CONFIG,
};

void dance_outer_top_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        swap_hands_toggle();
        reset_tap_dance(state);
    }
}

void dance_outer_top_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        layer_on(_MEDIA);
    }
}

void dance_outer_bottom_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        layer_on(_CONFIG);
        reset_tap_dance(state);
    }
}

void dance_outer_bottom_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        layer_on(_MEDIA);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SEARCH_SWAPHANDS] = ACTION_TAP_DANCE_FN_ADVANCED(dance_outer_top_each, dance_outer_top_finished, NULL),
    [TD_MEDIA_CONFIG] = ACTION_TAP_DANCE_FN_ADVANCED(dance_outer_bottom_each, dance_outer_bottom_finished, NULL),
};

/*
 * ┌────┬───┬───┬────┐
 * │  A │ B | C │ D  │
 * │    ├───┴───┤    │
 * ├────┤       ├────┤
 * │    │       │    │
 * │  E │       │ F  │
 * └────┘       └────┘
*/

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 5}, {0, 0}},
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        MS_BTN1, MS_BTN3, DRG_SCR, TD(TD_SEARCH_SWAPHANDS),
        MS_BTN1,                   TD(TD_MEDIA_CONFIG)
    ),
    [_MEDIA] = LAYOUT(
        KC_MNXT, KC_MUTE, PB_1,    TG(_MEDIA),
        KC_MPRV,                   KC_MPLY
    ),
    [_CONFIG] = LAYOUT(
        SET_DPI, XXXXXXX, XXXXXXX, XXXXXXX,
        HR_SCR,                    TG(_CONFIG)
    )
};
