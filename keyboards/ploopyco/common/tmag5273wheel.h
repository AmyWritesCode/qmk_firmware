/* Copyright 2025 Colin Lam, Ploopy Corporation (contact@ploopy.co)
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

#include <stdint.h>
#include "i2c_master.h"

#define TMAG5273A1_I2C_ADDRESS (0x35 << 1)
#define TMAG5273B1_I2C_ADDRESS (0x22 << 1)
#define TMAG5273C1_I2C_ADDRESS (0x78 << 1)
#define TMAG5273D1_I2C_ADDRESS (0x44 << 1)

#define TMAG5273_MAX_SENSOR_VALUE 5760

/* This value MUST be larger than 32 to function properly. */
#define TMAG5273_WHEEL_DEADZONE 128
#if TMAG5273_WHEEL_DEADZONE < 32
    #error "TMAG5273_WHEEL_DEADZONE must be greater than 32."
#endif
// The speed divisor decreases the speed. 1 is base speed; 2 is divided by 2,
// 3 is divided by 3, and so forth. For best results, make sure that
// TMAG5273_WHEEL_SPEED_DIV is an integer divisor of
// TMAG5273_WHEEL_DEADZONE (i.e. 3 is an integer divisor of 12, but
// 5 is not).
#define TMAG5273_VERTICAL_WHEEL_SPEED_DIV 4

/* Use different divisors for horizontal and vertical scrolls because they 
   get used for different applications. */
#define TMAG5273_HORIZONAL_WHEEL_SPEED_DIV 2

/* This value determines how far apart scroll wheel "ticks" are when
   emulating low-res scrolling. Tick units are in fractions of 
   TMAG5273_MAX_SENSOR_VALUE, which represents one physical revolution
   of the wheel. 480 = 12 ticks per revolution. */
#define TMAG5273_LOWRES_TICK_SIZE 480

/* This value is the same as TMAG5273_LOWRES_TICK_SIZE but applies to
   when the wheel is used to send left/right arrow keypresses for 
   horizontal scrolling. */
#define TMAG5273_HORIZ_SCROLL_TICK_SIZE 360

/* This value is the same as TMAG5273_LOWRES_TICK_SIZE but applies to
   when the wheel is used to send up/down arrow keypresses for 
   vertical scrolling. */
#define TMAG5273_VERT_SCROLL_TICK_SIZE 640

/* This value is the same as TMAG5273_LOWRES_TICK_SIZE but applies to
   when the wheel is used to adjust the volume. */
#define TMAG5273_VOLUME_SCROLL_TICK_SIZE 200

/* The time (in ms) to wait before shifting the deadzone a little to
   prevent spurious scroll events. */
#define TMAG5273_DEADZONE_PROTECTOR_TIMEOUT 512


#define REG_DEVICE_CONFIG_1         0x00
#define REG_DEVICE_CONFIG_2         0x01
#define REG_SENSOR_CONFIG_1         0x02
#define REG_SENSOR_CONFIG_2         0x03
#define REG_X_THR_CONFIG            0x04
#define REG_Y_THR_CONFIG            0x05
#define REG_Z_THR_CONFIG            0x06
#define REG_T_CONFIG                0x07
#define REG_INT_CONFIG_1            0x08
#define REG_MAG_GAIN_CONFIG         0x09
#define REG_MAG_OFFSET_CONFIG_1     0x0A
#define REG_MAG_OFFSET_CONFIG_2     0x0B
#define REG_I2C_ADDRESS             0x0C
#define REG_DEVICE_ID               0x0D
#define REG_MANUFACTURER_ID_LSB     0x0E
#define REG_MANUFACTURER_ID_MSB     0x0F
#define REG_T_MSB_RESULT            0x10
#define REG_T_LSB_RESULT            0x11
#define REG_X_MSB_RESULT            0x12
#define REG_X_LSB_RESULT            0x13
#define REG_Y_MSB_RESULT            0x14
#define REG_Y_LSB_RESULT            0x15
#define REG_Z_MSB_RESULT            0x16
#define REG_Z_LSB_RESULT            0x17
#define REG_CONV_STATUS             0x18
#define REG_ANGLE_RESULT_MSB        0x19
#define REG_ANGLE_RESULT_LSB        0x1A
#define REG_MAGNITUDE_RESULT        0x1B
#define REG_DEVICE_STATUS           0x1C

void tmag5273_init(void);
void tmag5273_init_device(uint8_t);
uint16_t tmag5273_get_angle(uint8_t);

int16_t calculate_wheel_delta( uint16_t wheel_rawangle, uint16_t wheel_current_position );
int16_t calculate_deadzone_distance( uint16_t wheel_rawangle, uint16_t* wheel_deadzone_center );
