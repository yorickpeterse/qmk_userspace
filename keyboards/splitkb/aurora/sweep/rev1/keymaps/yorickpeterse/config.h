/* Copyright 2022 splitkb.com <support@splitkb.com>
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

// Not yet available in `keymap.json` format
#ifdef RGB_MATRIX_ENABLE
#define RGB_MATRIX_SLEEP
#define RGB_MATRIX_KEYPRESSES
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define ENABLE_RGB_MATRIX_SOLID_SPLASH
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#endif

#undef IGNORE_MOD_TAP_INTERRUPT
#undef ONESHOT_TIMEOUT
#undef QMK_KEYS_PER_SCAN
#undef TAPPING_TERM
#undef DEBOUNCE
#undef MOUSEKEY_DELAY
#undef MOUSEKEY_TIME_TO_MAX
#undef MOUSEKEY_WHEEL_DELAY

#define RETRO_TAPPING
#define ONESHOT_TAP_TOGGLE 5
#define ONESHOT_TIMEOUT 3000
#define QMK_KEYS_PER_SCAN 4
#define TAPPING_TERM 100

#define COMBO_TERM 40
#define COMBO_TERM_PER_COMBO
#define COMBO_STRICT_TIMER
#define COMBO_MUST_PRESS_IN_ORDER_PER_COMBO
#define COMBO_SHOULD_TRIGGER

#define MK_KINETIC_SPEED
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_TIME_TO_MAX 60
#define MOUSEKEY_MOVE_DELTA 30
#define MOUSEKEY_INITIAL_SPEED 100
#define MOUSEKEY_BASE_SPEED 2400
#define MOUSEKEY_DECELERATED_SPEED 200
#define MOUSEKEY_ACCELERATED_SPEED 2400

#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 32
#define MOUSEKEY_WHEEL_MAX_SPEED 4
#define MOUSEKEY_WHEEL_TIME_TO_MAX 80

#define DEBOUNCE 5
#define USB_POLLING_INTERVAL_MS 1

// clang-format off
#define MAP( \
  L00, L01, L02, L03, L04,  R00, R01, R02, R03, R04, \
  L05, L06, L07, L08, L09,  R05, R06, R07, R08, R09, \
  L10, L11, L12, L13, L14,  R10, R11, R12, R13, R14, \
                 L15, L16,  R15, R16                 \
) LAYOUT_split_3x5_2( \
  KC_##L00, KC_##L01, KC_##L02, KC_##L03, KC_##L04,     KC_##R00, KC_##R01, KC_##R02, KC_##R03, KC_##R04, \
  KC_##L05, KC_##L06, KC_##L07, KC_##L08, KC_##L09,     KC_##R05, KC_##R06, KC_##R07, KC_##R08, KC_##R09, \
  KC_##L10, KC_##L11, KC_##L12, KC_##L13, KC_##L14,     KC_##R10, KC_##R11, KC_##R12, KC_##R13, KC_##R14, \
                                KC_##L15, KC_##L16,     KC_##R15, KC_##R16                                \
)
// clang-format on
