/* Copyright 2026 AmyWritesCode (@AmyWritesCode)
 * SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#undef ENABLE_COMPILE_KEYCODE

/* RGB effects */
/*
#undef RGBLIGHT_EFFECT_KNIGHT
#undef RGBLIGHT_EFFECT_SNAKE
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_EFFECT_TWINKLE
*/

/* RGB Layers */
#define RGBLIGHT_LAYERS
#define RGBLIGHT_MAX_LAYERS 4
#define RGBLIGHT_LAYER_BLINK
