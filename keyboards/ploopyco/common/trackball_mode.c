#include "print.h"
#include "trackball_mode.h"

bool is_drag_scroll   = false;
bool is_drag_select   = false;
bool is_volume_scroll = false;
bool is_hires_scroll  = true;
bool is_scroll_snap_v = false;
bool is_scroll_snap_h = false;

float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;
uint32_t last_scroll_time  = 0;

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
