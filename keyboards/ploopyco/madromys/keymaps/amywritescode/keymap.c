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

#include "amywritescode.h"
#include "tap_dance.h"
#include "trackball_mode.h"

/* ════════════════════ *
 *     Declarations     *
 * ════════════════════ */

enum layer {
    L_HAND,
    L_BASE,
    L_SELECT,
    L_SCROLL,
    L_MEDIA,
    L_CONFIG,
};

enum tap_dance_routine {
    TDR_MEDIA,
    TDR_SELECT_MOD_SWAP,
    TDR_EXIT_SAVE_CONFIG,
};

tap_dance_step_t tdr_media_current_step = TD_NONE;
tap_dance_step_t tdr_select_mod_swap_current_step = TD_NONE;
void tdr_media_each(tap_dance_state_t *state, void *user_data);
void tdr_media_finished(tap_dance_state_t *state, void *user_data);
void tdr_media_reset(tap_dance_state_t *state, void *user_data);

void tdr_select_mod_swap_each(tap_dance_state_t *state, void *user_data);
void tdr_select_mod_swap_finished(tap_dance_state_t *state, void *user_data);
void tdr_select_mod_swap_reset(tap_dance_state_t *state, void *user_data);

void tdr_exit_save_config_fn(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [TDR_MEDIA] = ACTION_TAP_DANCE_FN_ADVANCED(tdr_media_each, tdr_media_finished, tdr_media_reset),
    [TDR_SELECT_MOD_SWAP] = ACTION_TAP_DANCE_FN_ADVANCED( \
        tdr_select_mod_swap_each, tdr_select_mod_swap_finished, tdr_select_mod_swap_reset),
    [TDR_EXIT_SAVE_CONFIG] = ACTION_TAP_DANCE_FN(tdr_exit_save_config_fn),
};

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 5},{0, 0}},
};

/*
 * Layer flow:
 * L_HAND ────> L_BASE <──┬───> L_SELECT
 *                 ʌ      ├───> L_SCROLL ────> L_CONFIG
 *                 │      └───> L_MEDIA          │
 *                 └─────────────────────────────┘
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_HAND] = LAYOUT(
        XXX,                XXX,            XXX,            XXX,
        DF(L_BASE),                                         DF(L_BASE)
    ),
    [L_BASE] = LAYOUT(
        MS_BTN3,            TD(TDR_MEDIA),  DRAG_SCROLL,    MS_BTN2,
        MS_BTN1,                                            TD(TDR_SELECT_MOD_SWAP)
    ),
    [L_SELECT] = LAYOUT(
        TG(L_SELECT),        _______,        _______,       TG(L_SELECT),
        TG(L_SELECT),                                       TG(L_SELECT)
    ),
    [L_SCROLL] = LAYOUT(
        SCROLL_SNAP_V,      SCROLL_SNAP_H,  TG(L_SCROLL),   HIRES_SCROLL,
        _______,                                            TO(L_CONFIG)
    ),
    [L_MEDIA] = LAYOUT(
        KC_MNXT,            TG(L_MEDIA),    KC_MUTE,        XXX,
        KC_MPRV,                                            XXX
    ),
#ifdef DYNAMIC_TAPPING_TERM_ENABLE
    [L_CONFIG] = LAYOUT(
        DPI_CONFIG,         DT_UP,          DT_DOWN,        DT_PRNT,
        SCROLL_DIV_CONFIG,                                  TD(TDR_EXIT_SAVE_CONFIG)
    ),
#else
    [L_CONFIG] = LAYOUT(
        DPI_CONFIG,         XXX,            XXX,            XXX,
        SCROLL_DIV_CONFIG,                                  TD(TDR_EXIT_SAVE_CONFIG)
    ),
#endif
};

/* ═══════════════════════ *
 *     Core Processing     *
 * ═══════════════════════ */

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SAVE_SCROLL_CONFIG:
            return 5000;
        default:
            return TAPPING_TERM;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DF(L_BASE):
            if (record->event.key.col == 5) {
                // If the bottom-right button is pressed (left-click in LH mode), assume LH mode for this boot
                swap_hands_on();
            }
            break;
        case DRAG_SCROLL:
            if (record->event.pressed) {
                layer_on(L_SCROLL);
            } else {
                layer_off(L_SCROLL);
            }
            break;
        case LT(0, KC_NO):
            send_string("");
            break;
        default:
            break;
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Handle scroll modifiers here to ensure congruence with layer state
    switch (get_highest_layer(state)) {
        case L_MEDIA:
            set_volume_scroll(true);
            break;
        case L_CONFIG:
            // Reset scroll type to default
            set_volume_scroll(false);
            set_drag_scroll(false);
            break;
        default:
            set_volume_scroll(false);
            break;
    }

    return state;
}


/* ═════════════════ *
 *     Tap-dance     *
 * ═════════════════ */

void tdr_media_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        layer_move(L_MEDIA);
        state->finished = true;
    }
}

void tdr_media_finished(tap_dance_state_t *state, void *user_data) {
    tdr_media_current_step = get_current_dance_step(state);
    switch (tdr_media_current_step) {
        case TD_SINGLE_TAP:
            register_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            layer_on(L_MEDIA);
            break;
        default:
            break;
    }
}

void tdr_media_reset(tap_dance_state_t *state, void *user_data) {
    switch (tdr_media_current_step) {
        case TD_SINGLE_TAP:
            unregister_code(KC_MEDIA_PLAY_PAUSE);
            break;
        case TD_SINGLE_HOLD:
            layer_off(L_MEDIA);
            break;
        default:
            break;
    }
    tdr_media_current_step = TD_NONE;
}

void toggle_drag_select_user(void) {
    toggle_drag_select();
    if (is_drag_select) {
        layer_on(L_SELECT);
    } else {
        layer_off(L_SELECT);
    }
}


void tdr_select_mod_swap_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        swap_hands_toggle();
        state->finished = true;
    }
}

void tdr_select_mod_swap_finished(tap_dance_state_t *state, void *user_data) {
    tdr_select_mod_swap_current_step = get_current_dance_step(state);
    switch (tdr_select_mod_swap_current_step) {
        case TD_SINGLE_TAP:
            toggle_drag_select_user();
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_LEFT_GUI);
            break;
        default:
            break;
    }
}

void tdr_select_mod_swap_reset(tap_dance_state_t *state, void *user_data) {
    if (tdr_select_mod_swap_current_step == TD_SINGLE_HOLD) {
        unregister_code(KC_LEFT_GUI);
    }
    tdr_select_mod_swap_current_step = TD_NONE;
}

void tdr_exit_save_config_fn(tap_dance_state_t *state, void *user_data) {
    switch (get_current_dance_step(state)) {
        // TODO: replace with exit/save functionality
        case TD_SINGLE_HOLD:
            layer_clear();
            break;
        default:
            break;
    }
    reset_tap_dance(state);
}
