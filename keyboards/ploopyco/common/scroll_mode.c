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
#include "scroll_mode.h"

bool is_drag_scroll        = false;
bool is_drag_scroll_locked = false;
bool is_hires_scroll       = true;
bool is_scroll_snap_v      = false;
bool is_scroll_snap_h      = false;
bool is_volume_scroll      = false;

void set_drag_scroll(bool on) {
    if (is_drag_scroll != on) {
        is_drag_scroll = on;
        if (debug_mouse) {
            dprintf("Drag-scrolling is %s\n", is_drag_scroll ? "ON" : "OFF");
        }
    }
}

void toggle_drag_scroll(void) {
    set_drag_scroll(!is_drag_scroll);
}

void toggle_hires_scroll(void) {
    is_hires_scroll ^= 1;
    if (debug_mouse) {
        dprintf("Hi-res scrolling is %s\n", is_hires_scroll ? "ON" : "OFF");
    }
}

void toggle_scroll_snap_h(void) {
    is_scroll_snap_h ^= 1;
    if (debug_mouse) {
        dprintf("Horizontal snap is %s\n", is_scroll_snap_h ? "ON" : "OFF");
    }
    if (is_scroll_snap_h && is_scroll_snap_v) {
        is_scroll_snap_v = false;
        if (debug_mouse) {
            dprintf("Vertical snap is OFF\n");
        }
    }
}

void toggle_scroll_snap_v(void) {
    is_scroll_snap_v ^= 1;
    if (debug_mouse) {
        dprintf("Vertical snap is %s\n", is_scroll_snap_v ? "ON" : "OFF");
    }
    if (is_scroll_snap_v && is_scroll_snap_h) {
        is_scroll_snap_h = false;
        if (debug_mouse) {
            dprintf("Horizontal snap is OFF\n");
        }
    }
}

void set_volume_scroll(bool on) {
    if (is_volume_scroll != on) {
        is_volume_scroll = on;
        if (debug_mouse) {
            dprintf("Volume scrolling is %s\n", is_volume_scroll ? "ON" : "OFF");
        }
    }
}

void toggle_volume_scroll(void) {
    set_volume_scroll(!is_volume_scroll);
}
