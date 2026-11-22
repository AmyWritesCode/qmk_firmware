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
#include "trackball.h"

bool is_hires_scroll             = true;
bool is_volume_scroll            = false;
bool is_drag_scroll              = false;
bool is_scroll_axis_lock_h       = false;
bool is_scroll_axis_lock_v       = false;
snap_axis_t scroll_autosnap_axis = SNAP_AXIS_OFF;

#ifdef CONSOLE_ENABLE
static char * snap_axis_names[] = {
    #define TO_STR(name) #name,
        FOR_SNAP_AXIS(TO_STR)
    #undef TO_STR
};
#endif

void toggle_hires_scroll(void) {
    is_hires_scroll ^= 1;
    if (debug_mouse) {
        dprintf("Hi-res scrolling is %s\n", is_hires_scroll ? "ON" : "OFF");
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

void set_drag_scroll(bool on) {
    if (is_drag_scroll != on) {
        is_drag_scroll = on;
        if (debug_mouse) {
            dprintf("Drag-scrolling is %s\n", is_drag_scroll ? "ON" : "OFF");
        }
        reset_scroll_autosnap();
    }
}

void toggle_drag_scroll(void) {
    set_drag_scroll(!is_drag_scroll);
}

void set_scroll_axis_lock_h(bool on) {
    if (is_scroll_axis_lock_h != on) {
        is_scroll_axis_lock_h = on;
        if (debug_mouse) {
            dprintf("Horizontal snap is %s\n", is_scroll_axis_lock_h ? "ON" : "OFF");
        }
        if (is_scroll_axis_lock_h) {
            set_scroll_axis_lock_v(false);
            set_scroll_autosnap(SNAP_AXIS_OFF);
        }
    }
}

void toggle_scroll_axis_lock_h(void) {
    set_scroll_axis_lock_h(!is_scroll_axis_lock_h);
}

void set_scroll_axis_lock_v(bool on) {
    if (is_scroll_axis_lock_v != on) {
        is_scroll_axis_lock_v = on;
        if (debug_mouse) {
            dprintf("Vertical snap is %s\n", is_scroll_axis_lock_v ? "ON" : "OFF");
        }
        if (is_scroll_axis_lock_v) {
            set_scroll_axis_lock_h(false);
            set_scroll_autosnap(SNAP_AXIS_OFF);
        }
    }
}

void toggle_scroll_axis_lock_v(void) {
    set_scroll_axis_lock_v(!is_scroll_axis_lock_v);
}

void set_scroll_autosnap(snap_axis_t axis) {
    if (scroll_autosnap_axis != axis) {
        scroll_autosnap_axis = axis;
        if (debug_mouse) {
#ifdef CONSOLE_ENABLE
            dprintf("Auto-snap axis is %s\n", snap_axis_names[axis]);
#else
            dprintf("Auto-snap axis # is %u\n", axis);
#endif
        }
    }
}

void toggle_scroll_autosnap(void) {
    if (scroll_autosnap_axis == SNAP_AXIS_OFF) {
        set_scroll_autosnap(SNAP_AXIS_NONE);
    } else {
        set_scroll_autosnap(SNAP_AXIS_OFF);
    }
}

void reset_scroll_autosnap(void) {
    if (is_scroll_axis_snapped()) {
        set_scroll_autosnap(SNAP_AXIS_NONE);
    }
}

void clear_axis_lock(void) {
    set_scroll_axis_lock_h(false);
    set_scroll_axis_lock_v(false);
}

bool is_scroll_axis_snapped(void) {
    return scroll_autosnap_axis == SNAP_AXIS_HORZ || scroll_autosnap_axis == SNAP_AXIS_VERT;
}
