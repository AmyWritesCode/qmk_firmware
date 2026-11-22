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

#include <stdbool.h>
#include <stdint.h>

#define FOR_SNAP_AXIS(X) \
    X(SNAP_AXIS_OFF)     \
    X(SNAP_AXIS_NONE)    \
    X(SNAP_AXIS_HORZ)    \
    X(SNAP_AXIS_VERT)

#ifdef CONSOLE_ENABLE
typedef enum {
    #define TO_ENUM(name) name,
        FOR_SNAP_AXIS(TO_ENUM)
    #undef TO_ENUM
} snap_axis_t;
#endif

extern bool is_hires_scroll;
extern bool is_volume_scroll;
extern bool is_drag_scroll;
extern bool is_scroll_axis_lock_h;
extern bool is_scroll_axis_lock_v;
extern bool is_scroll_axis_snap;
extern snap_axis_t scroll_autosnap_axis;

void toggle_hires_scroll(void);
void set_volume_scroll(bool on);
void toggle_volume_scroll(void);
void set_drag_scroll(bool on);
void toggle_drag_scroll(void);
void toggle_scroll_axis_lock_h(void);
void set_scroll_axis_lock_h(bool on);
void toggle_scroll_axis_lock_v(void);
void set_scroll_axis_lock_v(bool on);
void set_scroll_autosnap(snap_axis_t axis);
void toggle_scroll_autosnap(void);
void reset_scroll_autosnap(void);
void clear_axis_lock(void);
bool is_scroll_axis_snapped(void);
