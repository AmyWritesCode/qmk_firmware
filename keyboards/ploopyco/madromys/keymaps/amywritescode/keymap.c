/* Copyright 2026 AmyWritesCode
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

#include "debug.h"
#include QMK_KEYBOARD_H
#include "layer_lock.h"
#include "process_tap_dance.h"
#include "ploopyco.h"
#include "scroll_mode.h"
#include "tap_dance.h"

/* ════════════════════ *
 *     Declarations     *
 * ════════════════════ */
typedef enum {
    L_BASE,
    L_SCROLL,
    L_MEDIA,
    L_CONFIG,
} layer_t;

enum td_keycodes_user {
    PLAY_PAUSE_MEDIA_ON,
    PLAY_PAUSE_MEDIA_OFF,
    PB_SWAP_HANDS,
    PB_HIRES_CONFIG_ON,
    EXIT_SAVE_CONFIG,
};

td_step_state_t td_play_pause_media_on_state = TD_NONE;
td_step_state_t td_play_pause_media_off_state = TD_NONE;
td_step_state_t td_pb_swap_hands_state = TD_NONE;
td_step_state_t td_pb_hires_config_on_state = TD_NONE;
td_step_state_t td_exit_save_config_step_state = TD_NONE;

void td_play_pause_media_on_finished(tap_dance_state_t *state, void *user_data);
void td_play_pause_media_on_reset(tap_dance_state_t *state, void *user_data);

void td_play_pause_media_off_finished(tap_dance_state_t *state, void *user_data);
void td_play_pause_media_off_reset(tap_dance_state_t *state, void *user_data);

void td_pb_swap_hands_finished(tap_dance_state_t *state, void *user_data);
void td_pb_swap_hands_reset(tap_dance_state_t *state, void *user_data);

void td_pb_hires_config_on_finished(tap_dance_state_t *state, void *user_data);
void td_pb_hires_config_on_reset(tap_dance_state_t *state, void *user_data);

void td_exit_save_config_finished(tap_dance_state_t *state, void *user_data);
void td_exit_save_config_reset(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [PLAY_PAUSE_MEDIA_ON] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_play_pause_media_on_finished, td_play_pause_media_on_reset),
    [PLAY_PAUSE_MEDIA_OFF] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_play_pause_media_off_finished, td_play_pause_media_off_reset),
    [PB_SWAP_HANDS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_pb_swap_hands_finished, td_pb_swap_hands_reset),
    [PB_HIRES_CONFIG_ON] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_pb_hires_config_on_finished, td_pb_hires_config_on_reset),
    [EXIT_SAVE_CONFIG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_exit_save_config_finished, td_exit_save_config_reset),
};

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  // {{0, 5},{0, 0}, {0, 4}, {0, 3}, {0, 2}, {0, 1}},
    {{5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}},
};

/*
 * Layer flow:
 *  L_BASE <──┬───> L_SCROLL ────> L_CONFIG
 *     ʌ      └───> L_MEDIA          │
 *     └─────────────────────────────┘
 *
 * Programmable Buttons:
 * 1. Single-tap {0,5}
 * 2. Double-tap {0,5}
 * 3. Single-tap L_SCROLL -> {0,5}
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT(
        MS_BTN3,    TD(PLAY_PAUSE_MEDIA_ON),    DRAG_SCROLL,    MS_BTN2,
        MS_BTN1,                                                TD(PB_SWAP_HANDS)
    ),
    [L_SCROLL] = LAYOUT(
        SCROLL_SNAP_V,  SCROLL_SNAP_H,  _______,    _______,
        _______,                                    TD(PB_HIRES_CONFIG_ON)
    ),
    [L_MEDIA] = LAYOUT(
        KC_MNXT,    _______,    KC_MUTE,    KC_MEDIA_FAST_FORWARD,
        KC_MPRV,                            KC_MEDIA_REWIND
    ),
#ifdef DYNAMIC_TAPPING_TERM_ENABLE
    [L_CONFIG] = LAYOUT(
        DT_UP,         DPI_CONFIG,  DRAG_SCROLL,    SCROLL_DIV_CONFIG,
        DT_DOWN,                                    TD(EXIT_SAVE_CONFIG)
    ),
#else
    [L_CONFIG] = LAYOUT(
        XXX,    PI_CONFIG,  DRAG_SCROLL,    SCROLL_DIV_CONFIG,
        XXX,                                TD(EXIT_SAVE_CONFIG)
    ),
#endif
};

/* ═══════════════════════ *
 *     Core Processing     *
 * ═══════════════════════ */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL:
            if (record->event.pressed) {
                layer_on(L_SCROLL);
            } else {
                layer_off(L_SCROLL);
            }
            break;
        case LT(L_CONFIG, QK_LLCK):
            if (record->tap.count && record->event.pressed) {
                tap_code16(QK_LLCK);
                return false;
            }
            break;
        case LT(L_BASE, SAVE_SCROLL_CONFIG):
            if (record->event.pressed) {
                if (record->tap.count) {
                    layer_lock_off(L_CONFIG);
                } else {
                    tap_code16(SAVE_SCROLL_CONFIG);
                }
            }
            return false;
        default:
            break;
    }

    return true;
}

void keyboard_post_init_user(void) {
    debug_enable=true;
    debug_matrix=true;
    //debug_keyboard=true;
    debug_mouse=true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef DYNAMIC_TAPPING_TERM_ENABLE
    if (debug_mouse && (keycode == DT_UP || keycode == DT_DOWN)) {
        dprintf("Tapping term: %u -> %u\n", \
            keycode == DT_UP
                ? g_tapping_term - DYNAMIC_TAPPING_TERM_INCREMENT
                : g_tapping_term + DYNAMIC_TAPPING_TERM_INCREMENT, \
            g_tapping_term \
        );
    }
#endif
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (debug_mouse) {
        dprintf("KL: Layer: %u\n", get_highest_layer(state));
    }

    if (get_highest_layer(state) == L_MEDIA) {
        set_volume_scroll(true);
    } else {
        set_volume_scroll(false);
    }

    return state;
}

/* ═════════════════ *
 *     Tap-dance     *
 * ═════════════════ */
void td_play_pause_media_on_finished(tap_dance_state_t *state, void *user_data) {
    td_play_pause_media_on_state = td_get_step_state(state);
    switch (td_play_pause_media_on_state) {
        case TD_SINGLE_TAP:
            register_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            layer_on(L_MEDIA);
            break;
        case TD_DOUBLE_TAP:
            if (!layer_state_is(L_MEDIA) || !is_layer_locked(L_MEDIA)) {
                layer_lock_invert(L_MEDIA);
            }
            break;
        default:
            break;
    }
}

void td_play_pause_media_on_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_play_pause_media_on_state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            layer_off(L_MEDIA);
            break;
        default:
            break;
    }
    td_play_pause_media_on_state = TD_NONE;
}

void td_play_pause_media_off_finished(tap_dance_state_t *state, void *user_data) {
    td_play_pause_media_off_state = td_get_step_state(state);
    switch (td_play_pause_media_off_state) {
        case TD_SINGLE_TAP:
            register_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_DOUBLE_TAP:
            if (layer_state_is(L_MEDIA)) {
                if (is_layer_locked(L_MEDIA)) {
                    layer_lock_invert(L_MEDIA);
                } else {
                    layer_off(L_MEDIA);
                }
            }
            break;
        default:
            break;
    }
}

void td_play_pause_media_off_reset(tap_dance_state_t *state, void *user_data) {
    if (td_play_pause_media_off_state == TD_SINGLE_TAP) {
        unregister_code(KC_MEDIA_PLAY_PAUSE);
    }
    td_play_pause_media_off_state = TD_NONE;
}

void td_pb_swap_hands_finished(tap_dance_state_t *state, void *user_data) {
    td_pb_swap_hands_state = td_get_step_state(state);
    switch (td_pb_swap_hands_state) {
        case TD_SINGLE_TAP:
            register_code16(PB_1);
            break;
        case TD_DOUBLE_TAP:
            register_code16(PB_2);
            break;
        case TD_TRIPLE_TAP:
            swap_hands_toggle();
            if (debug_mouse) {
                dprintf("Swap hands is %s\n", is_swap_hands_on() ? "ON" : "OFF");
            }
            break;
        default:
            break;
    }
}

void td_pb_swap_hands_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_pb_swap_hands_state) {
        case TD_SINGLE_TAP:
            unregister_code16(PB_1);
            break;
        case TD_TRIPLE_TAP:
            unregister_code16(PB_2);
            break;
        default:
            break;
    }
    td_pb_swap_hands_state = TD_NONE;
}

void td_pb_hires_config_on_finished(tap_dance_state_t *state, void *user_data) {
    td_pb_hires_config_on_state = td_get_step_state(state);
    switch (td_pb_hires_config_on_state) {
        case TD_SINGLE_TAP:
            register_code16(PB_3);
            break;
        case TD_DOUBLE_TAP:
            register_code16(HIRES_SCROLL);
            break;
        case TD_TRIPLE_TAP:
            layer_lock_on(L_CONFIG);
#ifdef CONSOLE_ENABLE
            printf("Config mode is enabled.\nTo save changes, press [Exit/Save] 3 times.\n"
                   "-----------------------------------------------------%s\n"
                   "Note: [*Drag scr*] is provided for testing purposes, and does NOT change any configurations.\n",
    #ifdef DYNAMIC_TAPPING_TERM_ENABLE
                is_swap_hands_on()
                    ? "| Scroll div | Drag scrl  | DPI        | Tap up     |\n"
                      "| Exit/Save  |                         | Tap down   |"
                    : "| Tap up     | DPI        | Drag scrl  | Scroll div |\n"
                      "| Tap down   |                         | Exit/Save  |"
            #else
                is_swap_hands_on()
                    ? "| Scroll div | *Drag scr* | DPI        |            |\n"
                      "| Exit/Save  |                         |            |"
                    : "|            | DPI        | *Drag scr* | Scroll div |\n"
                      "|            |                         | Exit/Save  |"
    #endif
            );
#endif
            break;
        default:
            break;
    }
}

void td_pb_hires_config_on_reset(tap_dance_state_t *state, void *user_data) {
    if (td_pb_hires_config_on_state == TD_SINGLE_TAP) {
        unregister_code16(PB_3);
    }
    td_pb_hires_config_on_state = TD_NONE;
}

void td_exit_save_config_finished(tap_dance_state_t *state, void *user_data) {
    if (debug_mouse) {
        dprintf("KL: td: EXIT_SAVE_CONFIG, pressed: %u, count: %u, int: %u\n", state->pressed, state->count, state->interrupted);
    }
    td_exit_save_config_step_state = td_get_step_state(state);
    switch (td_exit_save_config_step_state) {
        case TD_SINGLE_TAP:
            layer_lock_off(L_CONFIG);
            break;
        case TD_TRIPLE_TAP:
            register_code16(SAVE_SCROLL_CONFIG);
            if (layer_state_is(L_CONFIG)) {
                if (is_layer_locked(L_CONFIG)) {
                    layer_lock_invert(L_CONFIG);
                } else {
                    layer_off(L_CONFIG);
                }
            }
            layer_lock_off(L_CONFIG);
            break;
        default:
            break;
    }
}

void td_exit_save_config_reset(tap_dance_state_t *state, void *user_data) {
    if (td_exit_save_config_step_state == TD_TRIPLE_TAP) {
        unregister_code16(SAVE_SCROLL_CONFIG);
    }
    td_exit_save_config_step_state = TD_NONE;
}

