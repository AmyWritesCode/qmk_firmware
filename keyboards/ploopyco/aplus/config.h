/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
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

#define UNUSABLE_PINS \
    { GP6, GP10, GP16, GP18, GP19, GP20, GP22, GP24, GP25, GP26, GP28 }

#define POINTING_DEVICE_HIRES_SCROLL_ENABLE
#define POINTING_DEVICE_HIRES_SCROLL_MULTIPLIER 120
#define POINTING_DEVICE_HIRES_SCROLL_EXPONENT 0

#define POINTING_DEVICE_INVERT_Y

#define MOUSE_EXTENDED_REPORT
#define WHEEL_EXTENDED_REPORT

/* How far you have to move the cursor to register a gesture. */
#define POINTING_DEVICE_GESTURES_THRESHOLD 40

/* Uncomment these lines for pointing device acceleration. */
//#define POINTING_DEVICE_ACCEL_TAKEOFF 2.0      // lower/higher value = curve takes off more smoothly/abruptly
//#define POINTING_DEVICE_ACCEL_GROWTH_RATE 0.25 // lower/higher value = curve reaches its upper limit slower/faster
//#define POINTING_DEVICE_ACCEL_OFFSET 2.2       // lower/higher value = acceleration kicks in earlier/later
//#define POINTING_DEVICE_ACCEL_LIMIT 0.2        // lower limit of accel curve (minimum acceleration factor)

#define PLOOPY_DRAGSCROLL_MOMENTARY
#define PLOOPY_DRAGSCROLL_DIVISOR_H 1.0
#define PLOOPY_DRAGSCROLL_DIVISOR_V 0.3
#define PLOOPY_DRAGSCROLL_INVERT

/* PMW3360 Settings */
#define PMW33XX_LIFTOFF_DISTANCE 0x00
#define PMW33XX_CS_PIN GP5
#define SPI_SCK_PIN GP2
#define SPI_MISO_PIN GP4
#define SPI_MOSI_PIN GP3

#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP12
#define I2C1_SCL_PIN GP13

#define TMAG5273_D0_PWR_PIN GP9
#define TMAG5273_D1_PWR_PIN GP14

#define TMAG5273_D0_I2C_ADDRESS (0x50 << 1)
#define TMAG5273_D1_I2C_ADDRESS (0x51 << 1)

#define RGBLIGHT_SLEEP
#define RGBLIGHT_LAYERS_RETAIN_VAL
/* On-board lighting is configurable in 4 steps. This variable configures 
   the step size in fractions of full brightness (out of 255). */
#define RGBLIGHT_VAL_STEP 63
#if RGBLIGHT_VAL_STEP > 63
    #error "RGBLIGHT_VAL_STEP must be less than 64."
#endif

/* Define custom colours for effects */
#define HSV_NAVBLUE             180, 220, 252
#define HSV_NAVGREEN            66,  240, 252
#define HSV_GESTUREYELLOW       10,  250, 252
#define HSV_OPTIONCHANGED       240, 250, 252

/* How long to blink for when an option-change button is pressed. */
#define OPTION_CHANGE_BLINK_TIMEOUT 200 