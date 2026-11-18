/* Copyright 2026 AmyWritesCode
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
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

#include "ploopyco.h"
#include "quantum.h"
#include "report.h"
#include "scroll_mode.h"

// for legacy support
#if defined(OPT_DEBOUNCE) && !defined(PLOOPY_SCROLL_DEBOUNCE)
#    define PLOOPY_SCROLL_DEBOUNCE OPT_DEBOUNCE
#endif
#if defined(SCROLL_BUTT_DEBOUNCE) && !defined(PLOOPY_SCROLL_BUTTON_DEBOUNCE)
#    define PLOOPY_SCROLL_BUTTON_DEBOUNCE SCROLL_BUTT_DEBOUNCE
#endif

#ifndef PLOOPY_DPI_OPTIONS
#    define PLOOPY_DPI_OPTIONS \
        { 600, 900, 1200, 1600, 2400 }
#    ifndef PLOOPY_DPI_DEFAULT
#        define PLOOPY_DPI_DEFAULT 1
#    endif
#endif
#ifndef PLOOPY_DPI_DEFAULT
#    define PLOOPY_DPI_DEFAULT 0
#endif
#ifndef PLOOPY_SCROLL_DIV_OPTIONS
#    define PLOOPY_SCROLL_DIV_OPTIONS \
        { 0.5, 1.0, 1.5, 2.0, 4.0 }
#    ifndef PLOOPY_SCROLL_DIV_DEFAULT
#        define PLOOPY_SCROLL_DIV_DEFAULT 0
#    endif
#endif
#ifndef PLOOPY_SCROLL_DIV_DEFAULT
#    define PLOOPY_SCROLL_DIV_DEFAULT 0
#endif
#ifndef PLOOPY_DRAGSCROLL_H_COEF
#    define PLOOPY_DRAGSCROLL_H_COEF 1.0
#endif
#ifndef PLOOPY_VLMSCROLL_DEBOUNCE
#    define PLOOPY_VLMSCROLL_DEBOUNCE 50
#endif
#ifndef PLOOPY_HRSCROLL_DEBOUNCE
#    define PLOOPY_HRSCROLL_DEBOUNCE 16
#endif
#ifndef PLOOPY_INACTIVE_LAYER_TIMEOUT
#    define PLOOPY_INACTIVE_LAYER_TIMEOUT 20000
#endif

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
float             scroll_div_array[] = PLOOPY_SCROLL_DIV_OPTIONS;
#define DPI_OPTION_SIZE ARRAY_SIZE(dpi_array)
#define SCROLL_DIV_OPTION_SIZE ARRAY_SIZE(scroll_div_array)

// Trackball State
float scroll_accumulated_h         = 0;
float scroll_accumulated_v         = 0;
float scroll_accumulated_vlm       = 0;
uint16_t hr_scroll_debounce_timer  = 0;
uint16_t vlm_scroll_debounce_timer = 0;
uint16_t drag_scroll_press_timer   = 0;
bool mouse_moved                   = false;

void cycle_dpi(void);
void cycle_scroll_div(void);
int16_t get_emulated_standard_scroll_distance(float hires_scroll_distance, uint16_t resolution);
report_mouse_t process_historical_auto_snap(report_mouse_t mouse_report);

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    mouse_report = pointing_device_task_user(mouse_report);

    static uint16_t hires_scroll_resolution = 1;
    mouse_moved = mouse_report.h != 0 || mouse_report.v != 0;

    if (is_volume_scroll) {
        scroll_accumulated_h = 0;
        scroll_accumulated_v = 0;

        if(timer_elapsed(vlm_scroll_debounce_timer) > PLOOPY_VLMSCROLL_DEBOUNCE) {
            vlm_scroll_debounce_timer = timer_read32();
            if ((float) mouse_report.y < 0) {
                tap_code16(LSFT(KC_VOLU));
            } else if ((float) mouse_report.y > 0) {
                tap_code16(LSFT(KC_VOLD));
            }
        }

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (is_drag_scroll
#ifdef PLOOPY_DRAGSCROLL_SCROLLOCK
        || host_keyboard_led_state().scroll_lock
#endif
    ) {
        // Based on: https://github.com/adept-hires-scroll-mod/qmk_firmware
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

#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
        if (timer_elapsed32(hr_scroll_debounce_timer) < PLOOPY_HRSCROLL_DEBOUNCE) {
            mouse_report.h = 0;
            mouse_report.v = 0;
        } else {
            hr_scroll_debounce_timer = timer_read32();

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
                hires_scroll_resolution = pointing_device_get_hires_scroll_resolution();
                mouse_report.h = get_emulated_standard_scroll_distance(scroll_accumulated_h, hires_scroll_resolution);
    #ifdef PLOOPY_DRAGSCROLL_INVERT
                mouse_report.v = -get_emulated_standard_scroll_distance(scroll_accumulated_v, hires_scroll_resolution);
                scroll_accumulated_v += mouse_report.v;
    #else
                mouse_report.v = get_emulated_standard_scroll_distance(scroll_accumulated_v, hires_scroll_resolution);
                scroll_accumulated_v -= mouse_report.v;
    #endif
                scroll_accumulated_h -= mouse_report.h;
            }
        }
#else
            mouse_report.h = (int8_t)scroll_accumulated_h;
    #ifdef PLOOPY_DRAGSCROLL_INVERT
            mouse_report.v = -(int8_t)scroll_accumulated_v;
            scroll_accumulated_v += (int8_t)scroll_accumulated_v;
    #else
            mouse_report.v = (int8_t)scroll_accumulated_v;
            scroll_accumulated_v -= (int8_t)scroll_accumulated_v;
    #endif
            // Update accumulated scroll values by subtracting the integer parts
            scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
#endif

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return mouse_report;
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (debug_mouse) {
    #ifdef KEYCODE_STRING_ENABLE
        dprintf("KL: kc=0x%04X, kcs=%s, row/col=%2u/%2u, pressed=%u, int=%u, count=%u\n",
            keycode, get_keycode_string(keycode), record->event.key.row, record->event.key.col,
            record->event.pressed, record->tap.interrupted, record->tap.count
        );
    #else
        dprintf("KL: kc=0x%04X, row/col=%2u/%2u, pressed=%u, int=%u, count=%u\n",
            keycode, record->event.key.row, record->event.key.col,
            record->event.pressed, record->tap.interrupted, record->tap.count
        );
    #endif
    }

    if (!process_record_user(keycode, record)) {
        return false;
    }
    if (keycode == DRAG_SCROLL) {
#ifdef PLOOPY_DRAGSCROLL_MOMENTARY
        set_drag_scroll(record->event.pressed);
#else
        if (record->event.pressed) {
            toggle_drag_scroll();
        }
#endif
    } else if (keycode == DRAG_SCROLL_OR_CLICK3) {
        set_drag_scroll(record->event.pressed);
        if (record->event.pressed) {
            mouse_moved = false;
            scroll_accumulated_h = 0;
            scroll_accumulated_v = 0;
            drag_scroll_press_timer = timer_read();
        } else if (!mouse_moved && timer_elapsed(drag_scroll_press_timer) < TAPPING_TERM) {
            tap_code16(MS_BTN3);
            drag_scroll_press_timer = 0;
        }
    } else if (record->event.pressed) {
        switch (keycode) {
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
            case SCROLL_SNAP_OFF:
                set_scroll_snap_off();
                break;
            case SAVE_CONFIG:
#ifdef PLOOPY_MANUALLY_SAVE_CONFIG
                //eeconfig_update_kb(keyboard_config.raw);
                if (debug_mouse) {
                    dprint("Settings saved to EEPROM\n");
                }
#endif
                break;
            default:
                break;
        }
    }
    return true;
}

/* ═════════════════════════ *
 *     Resolution Config     *
 * ═════════════════════════ */

int16_t get_emulated_standard_scroll_distance(float hires_scroll_distance, uint16_t resolution) {
    return ((int16_t)(hires_scroll_distance / resolution)) * resolution;
}

void cycle_dpi(void) {
    uint8_t prev_config = keyboard_config.dpi_config;
    keyboard_config.dpi_config = (keyboard_config.dpi_config + 1) % DPI_OPTION_SIZE;
#ifndef PLOOPY_MANUALLY_SAVE_CONFIG
    eeconfig_update_kb(keyboard_config.raw);
#endif
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
    if (debug_mouse) {
        dprintf("dpi=%u->%u (%u->%u), resolution=%u\n",
            dpi_array[prev_config],
            dpi_array[keyboard_config.dpi_config],
            prev_config, keyboard_config.dpi_config,
            pointing_device_get_hires_scroll_resolution()
        );

    }
}

void cycle_scroll_div(void) {
    uint8_t prev_config = keyboard_config.scroll_div_config;
    keyboard_config.scroll_div_config = (keyboard_config.scroll_div_config + 1) % SCROLL_DIV_OPTION_SIZE;
#ifndef PLOOPY_MANUALLY_SAVE_CONFIG
    eeconfig_update_kb(keyboard_config.raw);
#endif
    if (debug_mouse) {
        dprintf("scroll-div=%.1f->%.1f (%u->%u)\n",
                scroll_div_array[prev_config],
                scroll_div_array[keyboard_config.scroll_div_config],
                prev_config, keyboard_config.scroll_div_config
        );
    }
}

/* ══════════════════════ *
 *     Hardware Setup     *
 * ══════════════════════ */

void keyboard_pre_init_kb(void) {
    // debug_enable  = true;
    // debug_matrix  = true;
    // debug_mouse   = true;
    // debug_encoder = true;

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
