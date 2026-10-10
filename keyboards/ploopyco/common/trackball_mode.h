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

extern bool is_drag_scroll;
extern bool is_drag_select;
extern bool is_volume_scroll;
extern bool is_hires_scroll;
extern bool is_scroll_snap_v;
extern bool is_scroll_snap_h;

void toggle_drag_select(void);
void set_drag_select(bool on);

void toggle_drag_scroll(void);
void set_drag_scroll(bool on);

void toggle_volume_scroll(void);
void set_volume_scroll(bool on);

void toggle_hires_scroll(void);
void toggle_scroll_snap_h(void);
void toggle_scroll_snap_v(void);
