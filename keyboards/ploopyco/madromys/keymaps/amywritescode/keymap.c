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
#include QMK_KEYBOARD_H
#include "process_tap_dance.h"
#include "ploopyco.h"
#include "scroll_mode.h"
#include "tap_dance.h"

/* ════════════════════ *
 *     Declarations     *
 * ════════════════════ */

// Layers
#define FOR_LAYERS(X) \
    X(L_BASE)         \
    X(L_SCROLL)       \
    X(L_MEDIA)        \
    X(L_CONFIG)

typedef enum {
#define TO_ENUM(name) name,
    FOR_LAYERS(TO_ENUM)
#undef TO_ENUM
} layer_t;

static char * layer_names[] = {
#define TO_STR(name) #name,
    FOR_LAYERS(TO_STR)
#undef TO_STR
};

// Tap-dances
enum td_keycodes_user {
    PLAY_PAUSE_MEDIA_TOGGLE,
    HYPR_MACROS,
    MEH_MACROS_SWAP_HANDS,
    SCROLL_SNAP_TOGGLE,
    CONFIG_ON,
    EXIT_OR_SAVE_CONFIG,
};

td_step_state_t td_play_pause_media_toggle_state = TD_NONE;
td_step_state_t td_hypr_macros_state = TD_NONE;
td_step_state_t td_meh_macros_swap_hands_state = TD_NONE;
td_step_state_t td_scroll_snap_toggle_state = TD_NONE;
td_step_state_t td_exit_or_save_config_state = TD_NONE;

void td_play_pause_media_toggle_finished(tap_dance_state_t *state, void *user_data);
void td_play_pause_media_toggle_reset(tap_dance_state_t *state, void *user_data);
void td_hypr_macros_finished(tap_dance_state_t *state, void *user_data);
void td_hypr_macros_reset(tap_dance_state_t *state, void *user_data);
void td_meh_macros_swap_hands_finished(tap_dance_state_t *state, void *user_data);
void td_meh_macros_swap_hands_reset(tap_dance_state_t *state, void *user_data);
void td_scroll_snap_toggle_finished(tap_dance_state_t *state, void *user_data);
void td_config_on_finished(tap_dance_state_t *state, void *user_data);
void td_exit_or_save_config_finished(tap_dance_state_t *state, void *user_data);
void td_exit_or_save_config_reset(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [PLAY_PAUSE_MEDIA_TOGGLE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_play_pause_media_toggle_finished, td_play_pause_media_toggle_reset),
    [HYPR_MACROS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_hypr_macros_finished, td_hypr_macros_reset),
    [MEH_MACROS_SWAP_HANDS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_meh_macros_swap_hands_finished, td_meh_macros_swap_hands_reset),
    [SCROLL_SNAP_TOGGLE] = ACTION_TAP_DANCE_FN(td_scroll_snap_toggle_finished),
    [CONFIG_ON] = ACTION_TAP_DANCE_FN(td_config_on_finished),
    [EXIT_OR_SAVE_CONFIG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_exit_or_save_config_finished, td_exit_or_save_config_reset),
};

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    {{5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}},
};

/*
 * Layer flow:
 * L_BASE <──┬───> L_MEDIA
 *    ʌ      └───> L_SCROLL ────┐
 *    └─────────── L_CONFIG <───┘
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT(
        TD(HYPR_MACROS),    TD(PLAY_PAUSE_MEDIA_TOGGLE),    DRAG_SCROLL_OR_CLICK3,  MS_BTN2,
        MS_BTN1,                                                                    TD(MEH_MACROS_SWAP_HANDS)
    ),
    [L_MEDIA] = LAYOUT(
        KC_MNXT,    _______,   KC_MUTE, KC_MEDIA_FAST_FORWARD,
        KC_MPRV,                        KC_MEDIA_REWIND
    ),
    [L_SCROLL] = LAYOUT(
        _______,    TD(SCROLL_SNAP_TOGGLE), _______,    _______,
        _______,                                        TD(CONFIG_ON)
    ),
    [L_CONFIG] = LAYOUT(
        SCROLL_DIV_CONFIG,  XXXXXXX,    DRAG_SCROLL,    DPI_CONFIG,
        HIRES_SCROLL,                                   TD(EXIT_OR_SAVE_CONFIG)
    ),
};

/* ═══════════════════════ *
 *     Core Processing     *
 * ═══════════════════════ */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(CONFIG_ON):
        case TD(EXIT_OR_SAVE_CONFIG):
            return 3000;
        case TD(SCROLL_SNAP_TOGGLE):
            return TAPPING_TERM * 2;
        default:
            return TAPPING_TERM;
    }
}

void keyboard_post_init_user(void) {
    debug_enable=true;
    // debug_matrix=true;
    // debug_keyboard=true;
    debug_mouse=true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL:
            if (!is_layer_locked(L_CONFIG)) {
                if (record->event.pressed) {
                    layer_on(L_SCROLL);
                } else {
                    layer_off(L_SCROLL);
                }
            }
            break;
        case DRAG_SCROLL_OR_CLICK3:
            if (record->event.pressed) {
                layer_on(L_SCROLL);
            } else {
                layer_off(L_SCROLL);
            }
            break;
        default:
            break;
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t current_layer = get_highest_layer(state);

    if (debug_mouse) {
        dprintf("Current layer: %u (%s)\n", current_layer, layer_names[current_layer]);
    }

    if (current_layer == L_MEDIA) {
        set_volume_scroll(true);
    } else {
        set_volume_scroll(false);
    }

    return state;
}

/* ═════════════════ *
 *     Tap-dance     *
 * ═════════════════ */
void td_play_pause_media_toggle_finished(tap_dance_state_t *state, void *user_data) {
    td_play_pause_media_toggle_state = td_get_step_state(state);
    switch (td_play_pause_media_toggle_state) {
        case TD_SINGLE_TAP:
            register_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            if (!is_layer_locked(L_MEDIA)) {
                layer_on(L_MEDIA);
            }
            break;
        case TD_DOUBLE_TAP:
            layer_lock_invert(L_MEDIA);
            break;
        default:
            break;
    }
}

void td_play_pause_media_toggle_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_play_pause_media_toggle_state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            if (!is_layer_locked(L_MEDIA)) {
                layer_off(L_MEDIA);
            }
            break;
        default:
            break;
    }
    td_play_pause_media_toggle_state = TD_NONE;
}

void td_hypr_macros_finished(tap_dance_state_t *state, void *user_data) {
    td_hypr_macros_state = td_get_step_state(state);
    switch (td_hypr_macros_state) {
        case TD_SINGLE_TAP:
            register_code16(HYPR(KC_1));
            break;
        case TD_SINGLE_HOLD:
            register_code16(HYPR(KC_2));
            break;
        case TD_DOUBLE_TAP:
            register_code16(HYPR(KC_3));
            break;
        case TD_TRIPLE_TAP:
            register_code16(HYPR(KC_4));
            break;
        default:
            break;
    }
}

void td_hypr_macros_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_hypr_macros_state) {
        case TD_SINGLE_TAP:
            unregister_code16(HYPR(KC_1));
            break;
        case TD_SINGLE_HOLD:
            unregister_code16(HYPR(KC_2));
            break;
        case TD_DOUBLE_TAP:
            unregister_code16(HYPR(KC_3));
            break;
        case TD_TRIPLE_TAP:
            unregister_code16(HYPR(KC_4));
            break;
        default:
            break;
    }
    td_hypr_macros_state = TD_NONE;
}

void td_meh_macros_swap_hands_finished(tap_dance_state_t *state, void *user_data) {
    td_meh_macros_swap_hands_state = td_get_step_state(state);
    switch (td_meh_macros_swap_hands_state) {
        case TD_SINGLE_TAP:
            register_code16(MEH(KC_1));
            break;
        case TD_SINGLE_HOLD:
            register_code16(MEH(KC_2));
            break;
        case TD_DOUBLE_TAP:
            register_code16(MEH(KC_3));
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

void td_meh_macros_swap_hands_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_meh_macros_swap_hands_state) {
        case TD_SINGLE_TAP:
            unregister_code16(MEH(KC_1));
            break;
        case TD_SINGLE_HOLD:
            unregister_code16(MEH(KC_2));
            break;
        case TD_DOUBLE_TAP:
            unregister_code16(MEH(KC_3));
            break;
        default:
            break;
    }
    td_meh_macros_swap_hands_state = TD_NONE;
}

void td_scroll_snap_toggle_finished(tap_dance_state_t *state, void *user_data) {
    switch (td_get_step_state(state)) {
        case TD_SINGLE_TAP:
            toggle_scroll_snap_v();
            break;
        case TD_SINGLE_HOLD:
            set_scroll_snap_off();
            break;
        case TD_DOUBLE_TAP:
            toggle_scroll_snap_h();
            break;
        default:
            break;
    }
    reset_tap_dance(state);
}

void td_config_on_finished(tap_dance_state_t *state, void *user_data) {
    if (td_get_step_state(state) == TD_SINGLE_HOLD) {
        layer_lock_on(L_CONFIG);
        // Print config layout
        char msg_out[101];
        sprintf(msg_out, is_swap_hands_on()
            ? "| DPI        | Drag scr | DPI      | Scroll div |\n"
              "| Exit/save  |                     | Hi-res scr |\n"
            : "| Scroll div |          | Drag scr | DPI        |\n"
              "| Hi-res scr |                     | Exit/save  |\n");
        if (debug_mouse) {
            dprintf("%s", msg_out);
        }
#ifdef SEND_STRING_ENABLE
        else {
            send_string(msg_out);
        }
#endif
    }

    reset_tap_dance(state);
}

void td_exit_or_save_config_finished(tap_dance_state_t *state, void *user_data) {
    if (debug_mouse) {
        dprintf("KL: td: EXIT_SAVE_CONFIG, pressed: %u, count: %u, int: %u\n", state->pressed, state->count, state->interrupted);
    }
    td_exit_or_save_config_state = td_get_step_state(state);
    switch (td_exit_or_save_config_state) {
        case TD_SINGLE_TAP:
            layer_lock_off(L_CONFIG);
            layer_off(L_CONFIG);
            break;
        case TD_SINGLE_HOLD:
            register_code16(SAVE_CONFIG);
            layer_lock_off(L_CONFIG);
            break;
        default:
            break;
    }
}

void td_exit_or_save_config_reset(tap_dance_state_t *state, void *user_data) {
    if (td_exit_or_save_config_state == TD_SINGLE_HOLD) {
        unregister_code16(SAVE_CONFIG);
    }
    td_exit_or_save_config_state = TD_NONE;
}
