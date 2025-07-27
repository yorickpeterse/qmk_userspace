#include "config.h"
#include "gpio.h"
#include QMK_KEYBOARD_H

#define KC_____ KC_TRNS
#define KC_NONE KC_NO
#define KC_NORM TO(NORMAL)
#define KC_PRIM RIGHT_THUMB
#define KC_NAV OSL(NAV)
#define KC_SECON MO(SECONDARY)
#define KC_MOUSE TG(MOUSE)
#define KC_FUNC OSL(FUNCTION)
#define KC_FULL LALT(KC_F11)
#define KC_LOCK LCTL(LALT(KC_DEL))
#define KC_CTL(KEY) LCTL(KC_##KEY)
#define KC_ALT(KEY) LALT(KC_##KEY)
#define KC_RESET QK_BOOT
#define KC_OCTL ONESHOT_CTL
#define KC_CAPS CW_TOGG
#define KC_STAB LSFT(KC_TAB)
#define LED_PIN 24
#define LED_VAL 32

// For some reason using _just_ KC_LGUI on an OSL layer results in it not
// working as it should, resulting in e.g. Gnome's overview not focusing the
// search bar. This lets us work around that.
#define KC_SUPER LGUI(KC_NONE)

enum custom_keycodes {
  RIGHT_THUMB = SAFE_RANGE,
  ONESHOT_CTL,
};

enum layer { NORMAL, PRIMARY, SECONDARY, NAV, FUNCTION, MOUSE };

enum oneshot_status {
  OS_DISABLED,
  OS_HOLDING,
  OS_RELEASED,
  OS_OTHER_KEY_PRESSED,
  OS_DISABLE,
};

struct oneshot_state {
  enum oneshot_status status;
  uint16_t modifier;
  int layer;
};

struct oneshot_state shift_state = {
    .status = OS_DISABLED,
    .modifier = KC_LSFT,
    .layer = PRIMARY,
};

struct oneshot_state ctl_state = {
    .status = OS_DISABLED,
    .modifier = KC_LCTL,
    .layer = -1,
};

bool disable_primary_after_caps_word = false;

void oneshot(struct oneshot_state *state, bool pressed) {
  if (pressed && state->status == OS_DISABLED) {
    state->status = OS_HOLDING;

    if (state->layer == -1) {
      register_code(state->modifier);
    } else {
      layer_on(state->layer);
    }
  } else {
    switch (state->status) {
    // Nothing pressed after the key down, so schedule the modifier for the
    // next key.
    case OS_HOLDING:
      state->status = OS_RELEASED;

      if (state->layer >= -1) {
        register_code(state->modifier);
      }

      break;
    // Another key was pressed, so we treat the key as a normal modifier you
    // need to hold down.
    case OS_OTHER_KEY_PRESSED:
      state->status = OS_DISABLED;

      if (state->layer == -1) {
        unregister_code(state->modifier);
      }

      break;
    default:
      state->status = OS_DISABLED;
      unregister_code(state->modifier);
      break;
    }

    // If a layer is used on hold, we should always disable it when releasing
    // the button.
    if (state->layer >= 0) {
      layer_off(state->layer);
    }
  }
}

void after_oneshot(struct oneshot_state *state, keyrecord_t *record) {
  if (record->event.pressed) {
    switch (state->status) {
    // If the modifier is held down, signal that another key is pressed.
    case OS_HOLDING:
      state->status = OS_OTHER_KEY_PRESSED;
      break;
    // If we are the first key down event after releasing the modifier (without
    // pressing another key), we'll be responsible for clearing the modifier.
    // This way the sequence `MOD -> A -> B` results in MOD only applying to A,
    // instead of applying to both A and B.
    case OS_RELEASED:
      state->status = OS_DISABLE;
      break;
    // If another key was pressed but is still held down, we need to disable the
    // modifier _first_ such that it only applies to the initially pressed key.
    case OS_DISABLE:
      state->status = OS_DISABLED;
      unregister_code(state->modifier);
    default:
      break;
    }
  } else {
    switch (state->status) {
    case OS_DISABLE:
      state->status = OS_DISABLED;
      unregister_code(state->modifier);
    default:
      break;
    }
  }
}

void process_oneshot_modifiers(keyrecord_t *record) {
  after_oneshot(&shift_state, record);
  after_oneshot(&ctl_state, record);
}

const key_override_t override_shift_dot =
    ko_make_basic(MOD_MASK_SHIFT, KC_DOT, KC_RALT);

const key_override_t *key_overrides[] = {
    &override_shift_dot,
};

enum combos {
  COMBO_ESC,
  COMBO_CTL,
  COMBO_NAV,
  COMBO_ENT,
};

uint32_t last_key_press = 0;

const uint16_t PROGMEM combo_esc[] = {KC_P, KC_F, COMBO_END};
const uint16_t PROGMEM combo_ctl[] = {KC_C, KC_D, COMBO_END};

const uint16_t PROGMEM combo_nav[] = {KC_U, KC_L, COMBO_END};
const uint16_t PROGMEM combo_ent[] = {KC_COMMA, KC_H, COMBO_END};

combo_t key_combos[] = {
    // Left
    [COMBO_ESC] = COMBO(combo_esc, KC_ESC),
    [COMBO_CTL] = COMBO(combo_ctl, KC_OCTL),

    // Right
    [COMBO_NAV] = COMBO(combo_nav, KC_NAV),
    [COMBO_ENT] = COMBO(combo_ent, KC_ENTER),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [NORMAL] = MAP(
        // -----------------------------------------      -----------------------------------------
               Q   ,   W   ,   F   ,   P   ,   B   ,          J   ,   L   ,   U   ,   Y   , BSPC  ,
        // --------+-------+-------+-------+--------      --------+-------+-------+-------+--------
               A   ,   R   ,   S   ,   T   ,   G   ,          M   ,   N   ,   E   ,   I   ,   O   ,
        // --------+-------+-------+-------+--------      --------+-------+-------+-------+--------
               Z   ,   X   ,   C   ,   D   ,   V   ,          K   ,   H   , COMMA ,  DOT  , CAPS  ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     MOUSE , SPACE ,         PRIM  ,  FUNC
        //                         --------+--------      ---------+---------
    ),

    [PRIMARY] = MAP(
        // -----------------------------------------      -----------------------------------------
             EXLM  , QUES  , LPRN  , PLUS  , AT    ,        BSLS  , EQUAL , RPRN  , GRAVE , ASTR  ,
        // -----------------------------------------      -----------------------------------------
             PIPE  , SLASH , LCBR  , MINUS , LABK  ,        RABK  , UNDS  , RCBR  , QUOTE , DLR   ,
        // -----------------------------------------      -----------------------------------------
             CIRC  , HASH  , LBRC  , SCLN  , AMPR  ,        TILD  , COLN  , RBRC  , DQUO  , PERC  ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     ____  , SECON ,         ____  ,  ____
        //                         --------+--------      ---------+---------
    ),

    [SECONDARY] = MAP(
        // -----------------------------------------      -----------------------------------------
             ____  , ____  , ____  , ____  , ____  ,        ____  , ____  , ____  , ____  , ____  ,
        // -----------------------------------------      -----------------------------------------
             1     , 2     , 3     , 4     , 5     ,        6     , 7     , 8     , 9     , 0     ,
        // -----------------------------------------      -----------------------------------------
             ____  , LALT  , STAB  , TAB   , ____  ,        ____  , ____  , COMMA , DOT   , ____  ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     ____  , ____  ,         ____  ,  ____
        //                         --------+--------      ---------+---------
    ),

    [NAV] = MAP(
        // -----------------------------------------      -----------------------------------------
             ____  , ____  , UP    , ____  , FULL  ,        LOCK  , ____  , ____  , ____  , ____  ,
        // -----------------------------------------      -----------------------------------------
             ____  , LEFT  , DOWN  , RIGHT , PGUP  ,        ____  ,CTL(F1),CTL(F2),CTL(F3),CTL(F4),
        // -----------------------------------------      -----------------------------------------
             ____  , CTL(1), CTL(2), CTL(3), PGDN  ,        ____  , ____  , ____  , ____  , ____  ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     ____  , ____  ,         SUPER ,  ____
        //                         --------+--------      ---------+---------
    ),

    [MOUSE] = MAP(
        // -----------------------------------------      -----------------------------------------
              ____ , CTL(C), CTL(V), ____  ,  ____ ,         ____ ,  ____ ,  ____ ,  ____ ,  ____ ,
        // -----------------------------------------      -----------------------------------------
              ____ , BTN3  , BTN2  , BTN1  , WH_U  ,         ____ ,  ____ ,  ____ ,  ____ ,  ____ ,
        // -----------------------------------------      -----------------------------------------
              ____ , LSFT  , LCTL  , ____  , WH_D  ,         ____ ,  ____ ,  ____ ,  ____ ,  ____ ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     ____  , ____  ,         ____  ,  ____
        //                         --------+--------      ---------+---------
    ),

    [FUNCTION] = MAP(
        // -----------------------------------------      -----------------------------------------
              ____ ,  ____ ,  ____ ,  ____ , ____  ,        ____  , PSCR  ,  ____ ,  ____ , ____  ,
        // -----------------------------------------      -----------------------------------------
              F1   ,  F2   ,  F3   ,  F4   ,  F5   ,         F6   ,  F7   ,  F8   ,  F9   ,  F10  ,
        // -----------------------------------------      -----------------------------------------
              F11  ,  F12  ,  ____ ,  ____ ,  ____ ,        ____  , ____  ,  ____ ,  ____ , RESET ,
        // -----------------------------------------      -----------------------------------------
        //                         --------+--------      ---------+---------
                                     ____  , ____  ,         ____  ,  ____
        //                         --------+--------      ---------+---------
    ),
};
// clang-format on

void led_blue(void) {
  rgblight_sethsv_noeeprom(170, 255, LED_VAL);
}

void led_pink(void) {
  rgblight_sethsv_noeeprom(234, 128, LED_VAL);
}

void led_off(void) {
  rgblight_sethsv_noeeprom(HSV_BLACK);
}

void keyboard_pre_init_user(void) {
  // Disable the power LED since it's way too bright.
  gpio_set_pin_output(LED_PIN);
  gpio_write_pin_high(LED_PIN);
}

void keyboard_post_init_user(void) {
  rgblight_enable_noeeprom();
  led_off();
  rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  last_key_press = timer_read32();

  switch (keycode) {
  case RIGHT_THUMB:
    // This ensures that enabling the primary layer while caps word is enabled
    // doesn't trigger an unregister of the shift key, disabling caps word in
    // the process.
    if (record->event.pressed && is_caps_word_on()) {
      layer_on(shift_state.layer);
      shift_state.status = OS_HOLDING;
      disable_primary_after_caps_word = true;
      return false;
    } else if (!record->event.pressed && disable_primary_after_caps_word) {
      // Allow disabling of caps word by tapping the right thumb.
      if (shift_state.status == OS_HOLDING) {
        caps_word_off();
      }

      shift_state.status = OS_DISABLED;
      layer_off(shift_state.layer);
      disable_primary_after_caps_word = false;
      return false;
    }

    oneshot(&shift_state, record->event.pressed);
    break;
  case ONESHOT_CTL:
    oneshot(&ctl_state, record->event.pressed);
    after_oneshot(&shift_state, record);
    break;
  case KC_FUNC:
    break;
  case KC_SPACE:
    // Key overrides that enable a layer don't work well with modifiers on the
    // target layer. For example, using an override for `Shift+Space` followed
    // by pressing Alt on the target layer seems to result in the layer being
    // deactivated _first_, then the normal key at the Alt position is
    // triggered.
    if (record->event.pressed && shift_state.status == OS_RELEASED &&
        ctl_state.status == OS_DISABLED) {
      shift_state.status = OS_DISABLED;
      unregister_code(shift_state.modifier);
      layer_on(SECONDARY);
      return false;
    } else if (!record->event.pressed && layer_state_is(SECONDARY)) {
      layer_off(SECONDARY);
      return false;
    }

    process_oneshot_modifiers(record);
    break;
  default:
    process_oneshot_modifiers(record);
    break;
  }

  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  switch (get_highest_layer(state)) {
  case MOUSE:
    led_blue();
    break;
  case FUNCTION:
    led_pink();
    break;
  default:
    led_off();
    break;
  }

  return state;
}

bool get_combo_must_press_in_order(uint16_t combo_index, combo_t *combo) {
  if (combo_index == COMBO_ENT) {
    return true;
  } else {
    return false;
  }
}

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
  if (combo_index == COMBO_ENT) {
    return 100;
  } else {
    return COMBO_TERM;
  }
}

uint32_t combo_idle_time(uint16_t index) {
  switch (index) {
  case COMBO_NAV:
    return 125;
  case COMBO_ENT:
  case COMBO_ESC:
    return 25;
  default:
    return 50;
  }
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo,
                          uint16_t keycode, keyrecord_t *record) {
  // The Enter combo is always enabled since it needs to be rolled in-order
  // anyway and allows for faster typing (e.g. for Vim commands).
  if (combo_index == COMBO_ENT) {
    return true;
  }

  // Other combos are disabled if the oneshot shift key is enabled. This way I
  // can type e.g. "Fp" without triggering an "fp" combo.
  if (shift_state.status != OS_DISABLED) {
    return false;
  }

  return timer_elapsed32(last_key_press) >= combo_idle_time(combo_index);
}
