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

#include "tap_dance.h"

td_step_state_t td_get_step_state(tap_dance_state_t *state) {
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
