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

void toggle_drag_scroll(void);
void set_drag_scroll(bool on);

void toggle_volume_scroll(void);
void set_volume_scroll(bool on);

void toggle_hires_scroll(void);
void toggle_scroll_snap_h(void);
void toggle_scroll_snap_v(void);
