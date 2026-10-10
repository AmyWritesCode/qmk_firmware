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

#include "print.h"
#include "trackball_mode.h"

bool is_drag_scroll   = false;
bool is_drag_select   = false;
bool is_volume_scroll = false;
bool is_hires_scroll  = true;
bool is_scroll_snap_v = false;
bool is_scroll_snap_h = false;

void toggle_drag_scroll(void) {
    is_drag_scroll ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Drag-scrolling is %s\n", is_drag_scroll ? "ON" : "OFF");
#endif
    if (is_drag_scroll && is_volume_scroll) {
        is_volume_scroll = false;
#ifdef CONSOLE_ENABLE
        uprintf("Volume scrolling is OFF\n");
#endif
    }
}

void set_drag_scroll(bool on) {
    if (is_drag_scroll != on) {
        toggle_drag_scroll();
    }
}

void toggle_drag_select(void) {
    is_drag_select ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Drag-select is %s\n", is_drag_scroll ? "ON" : "OFF");
#endif
}

void set_drag_select(bool on) {
    if (is_drag_select != on) {
        toggle_drag_select();
    }
}

void toggle_volume_scroll(void) {
    is_volume_scroll ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Volume scrolling is %s\n", is_volume_scroll ? "ON" : "OFF");
#endif
}

void set_volume_scroll(bool on) {
    if (is_volume_scroll != on) {
        toggle_volume_scroll();
    }
}

void toggle_hires_scroll(void) {
    is_hires_scroll ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Hi-res scrolling is %s\n", is_hires_scroll ? "ON" : "OFF");
#endif
}

void toggle_scroll_snap_h(void) {
    is_scroll_snap_h ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Horizontal snap is %s\n", is_scroll_snap_v ? "ON" : "OFF");
#endif
    if (is_scroll_snap_h && is_scroll_snap_v) {
        is_scroll_snap_v = false;
#ifdef CONSOLE_ENABLE
        uprintf("Vertical snap is OFF\n");
#endif
    }
}

void toggle_scroll_snap_v(void) {
    is_scroll_snap_h ^= 1;
#ifdef CONSOLE_ENABLE
    uprintf("Vertical snap is %s\n", is_scroll_snap_v ? "ON" : "OFF");
#endif
    if (is_scroll_snap_v && is_scroll_snap_h) {
        is_scroll_snap_h = false;
#ifdef CONSOLE_ENABLE
        uprintf("Horizontal snap is OFF\n");
#endif
    }
}
