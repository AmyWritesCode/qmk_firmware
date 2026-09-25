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
#include "bits.h"
#include QMK_KEYBOARD_H

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 5}, {0, 0}},
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_SEARCH_MEDIA] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_WSCH, LYR_MEDIA),
    [TD_SWAPHANDS_CONFIG] = ACTION_TAP_DANCE_FN_ADVANCED(td_swaphands_config_each, td_swaphands_config_finished, NULL),
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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LYR_BASE] = LAYOUT(
        MS_BTN1,        MS_BTN3,    DRAG_SCROLL,    TD(TD_SEARCH_MEDIA),
        MS_BTN1,                                    TD(TD_SWAPHANDS_CONFIG)
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
