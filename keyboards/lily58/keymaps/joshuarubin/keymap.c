#include QMK_KEYBOARD_H
#include "joshuarubin.h"

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

#ifdef RGBLIGHT_ENABLE
// Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

enum layers {
  _QWERTY,
  _LOWER, // NUMERIC
  _RAISE, // SYMBOL
  _DIRECTION,
  _ADJUST
};

#define LOWER    MO(_LOWER)
#define RAISE(X) LT(_RAISE, X)
#define DIR(X)   LT(_DIRECTION, X)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT(
  KC_GRV,         KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                    KC_6,           KC_7,    KC_8,    KC_9,    KC_0,         KC_MINS,
  KC_TAB,         KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                    KC_Y,           KC_U,    KC_I,    KC_O,    KC_P,         KC_BSLS,
  LCTL_T(KC_ESC), KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                    KC_H,           KC_J,    KC_K,    KC_L,    DIR(KC_SCLN), KC_QUOT,
  KC_LSFT,        KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_LBRC, KC_RBRC, KC_N,           KC_M,    KC_COMM, KC_DOT,  KC_SLSH,      KC_SFTENT,
                                   KC_LALT, KC_LGUI, LOWER, KC_ENT,  KC_SPC,  RAISE(KC_BSPC), KC_MINS, KC_EQL
),

[_LOWER] = LAYOUT(
  _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,         KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
  _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F6,         KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
  _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,          KC_7,    KC_8,    KC_9,    KC_0,    _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______,       KC_MUTE, _______, _______, _______, _______,
                             _______, _______, _______, _______, _______, RAISE(KC_DEL), KC_VOLD, KC_VOLU
),

[_RAISE] = LAYOUT(
  KC_TILD, KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,                      KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  _______,
  KC_TILD, KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,                      KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  _______,
  _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DQUO,
  _______, _______, _______, _______, _______, _______,  KC_LCBR, KC_RCBR,  _______, _______, KC_LT,   KC_GT,   KC_QUES, _______,
                             _______, _______, _______,  _______, _______,  _______, KC_UNDS, KC_PLUS
),

[_DIRECTION] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  _______, XXXXXXX, KC_HOME, KC_PGUP, KC_PGDN, KC_END,                      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, _______,  _______, _______,  KC_NDSH, KC_MDSH, _______, _______, SHRUG,   _______,
                             _______, _______, _______,  _______, _______,  _______, _______, _______
),

[_ADJUST] = LAYOUT(
  _______,  _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
  _______,  RESET,   DEBUG,   _______, _______, _______,                   _______, UC_MOD,  KC_INS,  _______, KC_PSCR, _______,
  RGB_MOD,  RGB_HUI, RGB_SAI, RGB_VAI, _______, _______,                   _______, _______, _______, _______, _______, _______,
  RGB_RMOD, RGB_HUD, RGB_SAD, RGB_VAD, _______, _______, KC_SLCK, KC_PAUS, AU_TOG,  MU_TOG,  MU_MOD,  _______, _______, _______,
                              RGB_TOG, AG_TOGG, _______, _______, _______, _______, MUV_DE,  MUV_IN
)

};

char layer_name[24];

const char *read_layer_name(void) {
  switch (get_highest_layer(layer_state)) {
  case _QWERTY:
    snprintf(layer_name, sizeof(layer_name), "Layer: Default");
    break;
  case _LOWER:
    snprintf(layer_name, sizeof(layer_name), "Layer: Lower");
    break;
  case _RAISE:
    snprintf(layer_name, sizeof(layer_name), "Layer: Raise");
    break;
  case _DIRECTION:
    snprintf(layer_name, sizeof(layer_name), "Layer: Direction");
    break;
  case _ADJUST:
    snprintf(layer_name, sizeof(layer_name), "Layer: Adjust");
    break;
  default:
    snprintf(layer_name, sizeof(layer_name), "Layer: Undef-%ld", layer_state);
  }

  return layer_name;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

int RGB_current_mode;

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    // SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb()); // turns on the display
    #endif
}

// SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
// const char *read_layer_state(void);
const char *read_logo(void);
const char *read_keylog(void);
const char *read_keylogs(void);

// void set_keylog(uint16_t keycode, keyrecord_t *record);
// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    // matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_layer_name());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
    // matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    // matrix_write_ln(matrix, read_host_led_state());
    // matrix_write_ln(matrix, read_timelog());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif // SSD1306OLED
