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
#include "tap_dance.h"
#include "trackball_mode.h"

/* ════════════════════ *
 *     Declarations     *
 * ════════════════════ */

typedef enum {
    L_BASE,
    L_HAND,
    L_SCROLL,
    L_MEDIA,
    L_CONFIG,
} layer_t;

enum td_keycodes_user {
    PLAY_STOP_MEDIA_TOGGLE,
    SELECT_P1_SWAP_HANDS_TOGGLE,
    EXIT_SAVE_CONFIG,
};

td_step_state_t td_play_stop_media_toggle_step_state = TD_NONE;
td_step_state_t td_select_p1_swap_hands_toggle_step_state = TD_NONE;

void td_play_stop_media_toggle_each(tap_dance_state_t *state, void *user_data);
void td_play_stop_media_toggle_finished(tap_dance_state_t *state, void *user_data);
void td_play_stop_media_toggle_reset(tap_dance_state_t *state, void *user_data);

void td_select_p1_swap_hands_toggle_each(tap_dance_state_t *state, void *user_data);
void td_select_p1_swap_hands_toggle_finished(tap_dance_state_t *state, void *user_data);
void td_select_p1_swap_hands_toggle_reset(tap_dance_state_t *state, void *user_data);

void td_exit_save_config_fn(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [PLAY_STOP_MEDIA_TOGGLE] = ACTION_TAP_DANCE_FN_ADVANCED( \
        td_play_stop_media_toggle_each, td_play_stop_media_toggle_finished, td_play_stop_media_toggle_reset),
    [SELECT_P1_SWAP_HANDS_TOGGLE] = ACTION_TAP_DANCE_FN_ADVANCED( \
        td_select_p1_swap_hands_toggle_each, td_select_p1_swap_hands_toggle_finished, td_select_p1_swap_hands_toggle_reset),
    [EXIT_SAVE_CONFIG] = ACTION_TAP_DANCE_FN(td_exit_save_config_fn),
};

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 5},{0, 0}},
};

uint32_t last_layer_interaction_time_media  = 0;
uint32_t last_layer_interaction_time_config = 0;

/*
 * Layer flow:
 * L_HAND ────> L_BASE <──┬───> L_SELECT
 *                 ʌ      ├───> L_SCROLL ────> L_CONFIG
 *                 │      └───> L_MEDIA          │
 *                 └─────────────────────────────┘
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT(
        MS_BTN3,    TD(SELECT_P1_SWAP_HANDS_TOGGLE),    LT(L_SCROLL, KC_SCRL),  MS_BTN2,
        MS_BTN1,                                                                TD(PLAY_STOP_MEDIA_TOGGLE)
    ),
    [L_HAND] = LAYOUT(
        XXX,        XXX,    XXX,    XXX,
        TO(L_BASE),                 SH_TOGG
    ),
    [L_SCROLL] = LAYOUT(
        SCROLL_SNAP_V,  SCROLL_SNAP_H,  TG(L_SCROLL),   HIRES_SCROLL,
        _______,                                        TO(L_CONFIG)
    ),
    [L_MEDIA] = LAYOUT(
        KC_MNXT,    XXX,    KC_MUTE,    XXX,
        KC_MPRV,                        TG(L_MEDIA)
    ),
#ifdef DYNAMIC_TAPPING_TERM_ENABLE
    [L_CONFIG] = LAYOUT(
        DPI_CONFIG,         DT_UP,  DT_DOWN,    DT_PRNT,
        SCROLL_DIV_CONFIG,                      TD(EXIT_SAVE_CONFIG)
    ),
#else
    [L_CONFIG] = LAYOUT(
        DPI_CONFIG,         XXX,    XXX,    XXX,
        SCROLL_DIV_CONFIG,                  TD(EXIT_SAVE_CONFIG)
    ),
#endif
};

/* ═══════════════════════ *
 *     Core Processing     *
 * ═══════════════════════ */

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(PLAY_STOP_MEDIA_TOGGLE):
        case TO(L_CONFIG):
            return 2000;
        case SAVE_SCROLL_CONFIG:
            return 5000;
        default:
            return TAPPING_TERM;
    }
}

uint16_t matrix_scan_set_layer_timeout(layer_t layer, int16_t timer) {
    if (layer_state_is(layer) && timer_elapsed(timer) > PLOOPY_INACTIVE_LAYER_TIMEOUT) {
        layer_off(layer);
        return 0;
    }
    return timer;
}

void matrix_scan_user(void) {
    last_layer_interaction_time_media = matrix_scan_set_layer_timeout(L_MEDIA, last_layer_interaction_time_media);
    last_layer_interaction_time_config = matrix_scan_set_layer_timeout(L_CONFIG, last_layer_interaction_time_config);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (layer_state_is(L_MEDIA)) {
        last_layer_interaction_time_media = timer_read();
    }
    if (layer_state_is(L_CONFIG)) {
        last_layer_interaction_time_media = timer_read();
    }

    switch (keycode) {
        case SH_TOGG:
            if (layer_state_is(L_HAND)) {
                layer_off(L_HAND);
            }
            break;
        case DRAG_SCROLL:
            if (record->event.pressed) {
                if (!layer_state_is(L_SCROLL)) {
                    layer_on(L_SCROLL);
                }
            } else if (layer_state_is(L_SCROLL)) {
                layer_off(L_SCROLL);
            }
            break;
        default:
            break;
    }

    return true;
}

void keyboard_post_init_user(void) {
    // Start on hand-selection layer
    // This allows the starting handedness to be set per session,
    // and removes the need for EEPROM writes
    layer_on(L_HAND);
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef DYNAMIC_TAPPING_TERM_ENABLE
    if (keycode == DT_UP || keycode == DT_DOWN) {
        uprintf("Tapping term: %u -> %u", \
            keycode == DT_UP ? g_tapping_term - DYNAMIC_TAPPING_TERM_INCREMENT : g_tapping_term + DYNAMIC_TAPPING_TERM_INCREMENT, \
            g_tapping_term \
        );
    }
#endif
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Handle scroll modifiers here to ensure congruence with layer state
    switch (get_highest_layer(state)) {
        case L_MEDIA:
            last_layer_interaction_time_media = timer_read();
            set_volume_scroll(true);
            set_drag_select(false);
            break;
        case L_CONFIG:
            last_layer_interaction_time_config = timer_read();
            // Reset scroll type to default
            set_volume_scroll(false);
            set_drag_scroll(false);
            set_drag_select(false);
            break;
        default:
            set_volume_scroll(false);
            set_drag_select(false);
            break;
    }

    return state;
}

/* ═════════════════ *
 *     Tap-dance     *
 * ═════════════════ */
void td_play_stop_media_toggle_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        layer_move(L_MEDIA);
        state->finished = true;
    }
}

void td_play_stop_media_toggle_finished(tap_dance_state_t *state, void *user_data) {
    td_play_stop_media_toggle_step_state = td_get_step_state(state);
    switch (td_play_stop_media_toggle_step_state) {
        case TD_SINGLE_TAP:
            register_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_MEDIA_STOP);
            break;
        default:
            break;
    }
}

void td_play_stop_media_toggle_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_play_stop_media_toggle_step_state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_MEDIA_STOP);
            break;
        default:
            break;
    }
    td_play_stop_media_toggle_step_state = TD_NONE;
}

void td_select_p1_swap_hands_toggle_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        swap_hands_toggle();
        state->finished = true;
    }
}

void td_select_p1_swap_hands_toggle_finished(tap_dance_state_t *state, void *user_data) {
    td_select_p1_swap_hands_toggle_step_state = td_get_step_state(state);
    switch (td_select_p1_swap_hands_toggle_step_state) {
        case TD_SINGLE_TAP:
            toggle_drag_select();
            break;
        case TD_SINGLE_HOLD:
            register_code16(PB_1);
            break;
        default:
            break;
    }
}

void td_select_p1_swap_hands_toggle_reset(tap_dance_state_t *state, void *user_data) {
    if (td_select_p1_swap_hands_toggle_step_state == TD_SINGLE_HOLD) {
        unregister_code16(PB_1);
    }
    td_select_p1_swap_hands_toggle_step_state = TD_NONE;
}

void td_exit_save_config_fn(tap_dance_state_t *state, void *user_data) {
    switch (td_get_step_state(state)) {
        // TODO: replace with exit/save functionality
        case TD_SINGLE_HOLD:
            layer_clear();
            break;
        default:
            break;
    }
    reset_tap_dance(state);
}
