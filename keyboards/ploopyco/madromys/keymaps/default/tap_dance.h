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
    TD_SEARCH_SWAPHANDS_CONFIG,
};


typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,         // Equivalent to: double-tap + hold on 2nd tap
    TD_DOUBLE_SINGLE_TAP,   // Equivalent to: 2 single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD          // Equivalent to: triple-tap + hold on 3rd tap
} td_outcome_t;

typedef struct {
    bool is_press_action;
    td_outcome_t outcome;
} td_tap_t;
