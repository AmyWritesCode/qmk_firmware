/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
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

#include "analog.h"
#include "config_defaults.h"
#include "ploopyco.h"

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
float             scroll_div_array[] = PLOOPY_SCROLL_DIV_OPTIONS;
#define DPI_OPTION_SIZE ARRAY_SIZE(dpi_array)
#define SCROLL_DIV_OPTION_SIZE ARRAY_SIZE(scroll_div_array)

// Trackball State
bool  is_drag_scroll       = false;
bool  is_drag_select       = false;
bool  is_volume_scroll     = false;
bool  is_hires_scroll      = true;
bool  is_scroll_snap_v     = false;
bool  is_scroll_snap_h     = false;
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;
uint32_t last_scroll_time  = 0;

void toggle_drag_scroll(void) {
    is_drag_scroll ^= 1;
    if (is_drag_scroll) {
        is_volume_scroll = false;
    }
}

void toggle_drag_select(void) {
    is_drag_select ^= 1;
}

void toggle_volume_scroll(void) {
    is_volume_scroll ^= 1;
}

void toggle_hires_scroll(void) {
    is_hires_scroll ^= 1;
}

void toggle_scroll_snap_h(void) {
    is_scroll_snap_h ^= 1;
    if (is_scroll_snap_h) {
        is_scroll_snap_v = false;
    }
}

void toggle_scroll_snap_v(void) {
    is_scroll_snap_v ^= 1;
    if (is_scroll_snap_v) {
        is_scroll_snap_h = false;
    }
}

void cycle_dpi(void) {
    uint8_t prev_dpi = keyboard_config.dpi_config;
    keyboard_config.dpi_config = (keyboard_config.dpi_config + 1) % DPI_OPTION_SIZE;
#ifndef PLOOPY_CONFIRM_UPDATE_EEPROM
    eeconfig_update_kb(keyboard_config.raw);
#endif
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
    printf("DPI / Scroll Div.: %d -> %d / %d", \
        prev_dpi, keyboard_config.dpi_config, keyboard_config.scroll_div_config);
}

void cycle_scroll_div(void) {
    uint8_t prev_scroll_div = keyboard_config.scroll_div_config;
    keyboard_config.scroll_div_config = (keyboard_config.scroll_div_config + 1) % SCROLL_DIV_OPTION_SIZE;
#ifndef PLOOPY_CONFIRM_UPDATE_EEPROM
    eeconfig_update_kb(keyboard_config.raw);
#endif
    printf("DPI / Scroll Div.: %d / %d -> %d", \
        keyboard_config.dpi_config, prev_scroll_div, keyboard_config.scroll_div_config);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    static uint16_t hires_scroll_res = 1;

    if (is_volume_scroll) {
        if(timer_elapsed(last_scroll_time) > PLOOPY_VLMSCROLL_DEBOUNCE) {
            if ((float) mouse_report.y < 0) {
                tap_code(KC_VOLU);
            } else if ((float) mouse_report.y > 0) {
                tap_code(KC_VOLD);
            }

            last_scroll_time = timer_read();
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    }
#ifdef PLOOPY_DRAGSCROLL_SCROLLOCK
    else if (is_drag_scroll || host_keyboard_led_state().scroll_lock) {
#else
    else if (is_drag_scroll) {
#endif
        hires_scroll_res = pointing_device_get_hires_scroll_resolution();

        if (is_drag_select) {
            mouse_report.buttons |= MOUSE_BTN1;
        }

        if (is_scroll_snap_v) {
            scroll_accumulated_h = 0;
        } else {
            scroll_accumulated_h += (float)mouse_report.x / scroll_div_array[keyboard_config.scroll_div_config] * PLOOPY_DRAGSCROLL_H_COEF;
        }

        if (is_scroll_snap_h) {
            scroll_accumulated_v = 0;
        } else {
            scroll_accumulated_v += (float)mouse_report.y / scroll_div_array[keyboard_config.scroll_div_config];
        }

        // Throttle scroll reporting rate to reasonable value
        if (timer_elapsed32(last_scroll_time) < PLOOPY_HRSCROLL_DEBOUNCE) {
            mouse_report.h = 0;
            mouse_report.v = 0;
        } else {
            last_scroll_time = timer_read32();

            if (is_hires_scroll) {
                // Assign integer parts of accumulated scroll values to the mouse report
                mouse_report.h = (int16_t)scroll_accumulated_h;
#ifdef PLOOPY_DRAGSCROLL_INVERT
                mouse_report.v = -(int16_t)scroll_accumulated_v;
#else
                mouse_report.v = (int16_t)scroll_accumulated_v;
#endif
                // Update accumulated scroll values by subtracting the integer parts
                scroll_accumulated_h -= (int16_t)scroll_accumulated_h;
                scroll_accumulated_v -= (int16_t)scroll_accumulated_v;

            } else {
                // Emulate no hires scrolling by only reporting in increments of the resolution
                mouse_report.h = (int16_t)scroll_accumulated_h / hires_scroll_res * hires_scroll_res;
#ifdef PLOOPY_DRAGSCROLL_INVERT
                mouse_report.v = -(int16_t)scroll_accumulated_v / hires_scroll_res * hires_scroll_res;
                scroll_accumulated_v += mouse_report.v;
#else
                mouse_report.v = (int16_t)scroll_accumulated_v / hires_scroll_res * hires_scroll_res;
                scroll_accumulated_v -= mouse_report.v;
#endif
                scroll_accumulated_h -= mouse_report.h;
            }
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return pointing_device_task_user(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (debug_mouse) {
        dprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
    }

    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (keycode == DRAG_SCROLL) {
        if (record->tap.count && record->event.pressed) {
            toggle_drag_scroll();
        } else {
            is_drag_scroll = record->event.pressed;
        }
    } else if (keycode == VOLUME_SCROLL) {
        if (record->tap.count && record->event.pressed) {
            toggle_volume_scroll();
        } else {
            is_volume_scroll = record->event.pressed;
        }
    } else if (record->event.pressed) {
        switch(keycode) {
            case DPI_CONFIG:
                cycle_dpi();
                break;
            case SCROLL_DIV_CONFIG:
                cycle_scroll_div();
                break;
            case HIRES_SCROLL:
                toggle_hires_scroll();
                break;
            case SCROLL_SNAP_H:
                toggle_scroll_snap_h();
                break;
            case SCROLL_SNAP_V:
                toggle_scroll_snap_v();
                break;
            case SAVE_SCROLL_CONFIG:
#ifdef PLOOPY_CONFIRM_UPDATE_EEPROM
                eeconfig_update_kb(keyboard_config.raw);
#endif
                break;
            default:
                break;
        }
    }
    return true;
}

// Hardware Setup
void keyboard_pre_init_kb(void) {
    // debug_enable  = true;
    // debug_matrix  = true;
    // debug_mouse   = true;

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
#ifdef UNUSABLE_PINS
    const pin_t unused_pins[] = UNUSABLE_PINS;

    for (uint8_t i = 0; i < ARRAY_SIZE(unused_pins); i++) {
        gpio_set_pin_output_push_pull(unused_pins[i]);
        gpio_write_pin_low(unused_pins[i]);
    }
#endif

    // This is the debug LED.
#if defined(DEBUG_LED_PIN)
    gpio_set_pin_output_push_pull(DEBUG_LED_PIN);
    gpio_write_pin(DEBUG_LED_PIN, debug_enable);
#endif

    keyboard_pre_init_user();
}

void pointing_device_init_kb(void) {
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    else if (keyboard_config.scroll_div_config > SCROLL_DIV_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
}

void eeconfig_init_kb(void) {
    keyboard_config.dpi_config = PLOOPY_DPI_DEFAULT;
    keyboard_config.scroll_div_config = PLOOPY_SCROLL_DIV_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}
