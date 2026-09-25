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
#pragma once

#include "process_tap_dance.h"

enum tap_dances {
    TD_SEARCH_MEDIA,
    TD_SWAPHANDS_CONFIG,
    TD_BOOT,
};

enum layers {
    LYR_BASE,
    LYR_MEDIA,
    LYR_CONFIG,
};

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

void td_swaphands_config_each(tap_dance_state_t *state, void *user_data);
void td_swaphands_config_finished(tap_dance_state_t *state, void *user_data);
