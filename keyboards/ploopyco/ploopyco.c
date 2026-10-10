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

#include "analog.h"
#include "ploopyco.h"
#include "trackball_mode.h"

/* ════════════════════ *
 *     Declarations     *
 * ════════════════════ */

// Clear legacy configs
#undef PLOOPY_SCROLL_DEBOUNCE
#undef PLOOPY_SCROLL_BUTTON_DEBOUNCE

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
#ifndef PLOOPY_HRSCROLL_DEBOUNCE
#    define PLOOPY_HRSCROLL_DEBOUNCE 16
#endif
#ifndef PLOOPY_VLMSCROLL_DEBOUNCE
#    define PLOOPY_VLMSCROLL_DEBOUNCE 50
#endif
#ifndef PLOOPY_INACTIVE_LAYER_TIMEOUT
#    define PLOOPY_INACTIVE_LAYER_TIMEOUT 20000
#endif

keyboard_config_t keyboard_config;
keyboard_config_t keyboard_config_saved;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
float             scroll_div_array[] = PLOOPY_SCROLL_DIV_OPTIONS;
#define DPI_OPTION_SIZE ARRAY_SIZE(dpi_array)
#define SCROLL_DIV_OPTION_SIZE ARRAY_SIZE(scroll_div_array)

// Trackball State
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;
uint16_t last_scroll_time  = 0;

/* ═══════════════════════ *
 *     Core Processing     *
 * ═══════════════════════ */

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    static uint16_t hires_scroll_resolution = 1;

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
    } else if (is_drag_scroll
#ifdef PLOOPY_DRAGSCROLL_SCROLLOCK
        || host_keyboard_led_state().scroll_lock
#endif
    ) {
        // Based on: https://github.com/adept-hires-scroll-mod/qmk_firmware
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
                hires_scroll_resolution = pointing_device_get_hires_scroll_resolution();
                mouse_report.h = (int16_t)scroll_accumulated_h * hires_scroll_resolution;
#ifdef PLOOPY_DRAGSCROLL_INVERT
                mouse_report.v = -(int16_t)scroll_accumulated_v * hires_scroll_resolution;
#else
                mouse_report.v = (int16_t)scroll_accumulated_v * hires_scroll_resolution;
#endif
                scroll_accumulated_v -= mouse_report.v;
                scroll_accumulated_h -= mouse_report.h;
            }
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return pointing_device_task_user(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
#ifdef CONSOLE_ENABLE
    uprintf("KL: kc={0x%04X,%s}, row/col={%2u,%2u}, pressed=%u, time=%5u, int=%u, count=%u\n", \
        keycode, get_keycode_string(keycode), record->event.key.row, record->event.key.col, \
        record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count \
    );
#endif

    if (!process_record_user(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case DRAG_SCROLL:
#ifdef PLOOPY_DRAGSCROLL_MOMENTARY
            set_drag_scroll(record->event.pressed);
#else
            toggle_drag_scroll();
#endif
            break;
        case DRAG_SELECT:
            toggle_drag_select();
            break;
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
    return true;
}

/* ═════════════════════════ *
 *     Resolution Config     *
 * ═════════════════════════ */

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

/* ══════════════════════ *
 *     Hardware Setup     *
 * ══════════════════════ */

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
