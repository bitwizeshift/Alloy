// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/extra/sdl2-bindings/sdl2_pump_source.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include "alloy/core/assert.hpp"

#include "alloy/platform/events/window_events.hpp"
#include "alloy/platform/keyboard.hpp"
#include "alloy/platform/events/keyboard_events.hpp"
#include "alloy/platform/events/mouse_events.hpp"
#include "alloy/platform/message_pump.hpp"

#include <SDL3/SDL.h>

#include <cstdint> // std::int32_t
#include <optional>


namespace {

auto is_window_event(const ::SDL_Event& event) -> bool
{
  return event.type >= SDL_EVENT_WINDOW_FIRST &&
         event.type <= SDL_EVENT_WINDOW_LAST;
}

auto to_mouse_button(::Uint8 code) -> std::optional<alloy::platform::mouse::button>
{
  switch (code) {
    case SDL_BUTTON_LEFT: {
      return alloy::platform::mouse::button::left;
    }
    case SDL_BUTTON_RIGHT: {
      return alloy::platform::mouse::button::right;
    }
    case SDL_BUTTON_MIDDLE: {
      return alloy::platform::mouse::button::middle;
    }
    default: {
      break;
    }
  }
  return std::nullopt;
}

auto to_scan_code(::SDL_Scancode code) -> alloy::platform::keyboard::scan_code
{
  switch (code) {
    //-------------------------------------------------------------------------
    // Function Keys
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_F1: {
      return alloy::platform::keyboard::scan_code::f1;
    }
    case SDL_SCANCODE_F2: {
      return alloy::platform::keyboard::scan_code::f2;
    }
    case SDL_SCANCODE_F3: {
      return alloy::platform::keyboard::scan_code::f3;
    }
    case SDL_SCANCODE_F4: {
      return alloy::platform::keyboard::scan_code::f4;
    }
    case SDL_SCANCODE_F5: {
      return alloy::platform::keyboard::scan_code::f5;
    }
    case SDL_SCANCODE_F6: {
      return alloy::platform::keyboard::scan_code::f6;
    }
    case SDL_SCANCODE_F7: {
      return alloy::platform::keyboard::scan_code::f7;
    }
    case SDL_SCANCODE_F8: {
      return alloy::platform::keyboard::scan_code::f8;
    }
    case SDL_SCANCODE_F9: {
      return alloy::platform::keyboard::scan_code::f9;
    }
    case SDL_SCANCODE_F10: {
      return alloy::platform::keyboard::scan_code::f10;
    }
    case SDL_SCANCODE_F11: {
      return alloy::platform::keyboard::scan_code::f11;
    }
    case SDL_SCANCODE_F12: {
      return alloy::platform::keyboard::scan_code::f12;
    }
    case SDL_SCANCODE_F13: {
      return alloy::platform::keyboard::scan_code::f13;
    }
    case SDL_SCANCODE_F14: {
      return alloy::platform::keyboard::scan_code::f14;
    }
    case SDL_SCANCODE_F15: {
      return alloy::platform::keyboard::scan_code::f15;
    }
    case SDL_SCANCODE_F16: {
      return alloy::platform::keyboard::scan_code::f16;
    }
    case SDL_SCANCODE_F17: {
      return alloy::platform::keyboard::scan_code::f17;
    }
    case SDL_SCANCODE_F18: {
      return alloy::platform::keyboard::scan_code::f18;
    }
    case SDL_SCANCODE_F19: {
      return alloy::platform::keyboard::scan_code::f19;
    }
    case SDL_SCANCODE_F20: {
      return alloy::platform::keyboard::scan_code::f20;
    }
    case SDL_SCANCODE_F21: {
      return alloy::platform::keyboard::scan_code::f21;
    }
    case SDL_SCANCODE_F22: {
      return alloy::platform::keyboard::scan_code::f22;
    }
    case SDL_SCANCODE_F23: {
      return alloy::platform::keyboard::scan_code::f23;
    }
    case SDL_SCANCODE_F24: {
      return alloy::platform::keyboard::scan_code::f24;
    }

    //-------------------------------------------------------------------------
    // Number row
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_0: {
      return alloy::platform::keyboard::scan_code::number_0;
    }
    case SDL_SCANCODE_1: {
      return alloy::platform::keyboard::scan_code::number_1;
    }
    case SDL_SCANCODE_2: {
      return alloy::platform::keyboard::scan_code::number_2;
    }
    case SDL_SCANCODE_3: {
      return alloy::platform::keyboard::scan_code::number_3;
    }
    case SDL_SCANCODE_4: {
      return alloy::platform::keyboard::scan_code::number_4;
    }
    case SDL_SCANCODE_5: {
      return alloy::platform::keyboard::scan_code::number_5;
    }
    case SDL_SCANCODE_6: {
      return alloy::platform::keyboard::scan_code::number_6;
    }
    case SDL_SCANCODE_7: {
      return alloy::platform::keyboard::scan_code::number_7;
    }
    case SDL_SCANCODE_8: {
      return alloy::platform::keyboard::scan_code::number_8;
    }
    case SDL_SCANCODE_9: {
      return alloy::platform::keyboard::scan_code::number_9;
    }

    //-------------------------------------------------------------------------
    // Letters
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_A: {
      return alloy::platform::keyboard::scan_code::letter_a;
    }
    case SDL_SCANCODE_B: {
      return alloy::platform::keyboard::scan_code::letter_b;
    }
    case SDL_SCANCODE_C: {
      return alloy::platform::keyboard::scan_code::letter_c;
    }
    case SDL_SCANCODE_D: {
      return alloy::platform::keyboard::scan_code::letter_d;
    }
    case SDL_SCANCODE_E: {
      return alloy::platform::keyboard::scan_code::letter_e;
    }
    case SDL_SCANCODE_F: {
      return alloy::platform::keyboard::scan_code::letter_f;
    }
    case SDL_SCANCODE_G: {
      return alloy::platform::keyboard::scan_code::letter_g;
    }
    case SDL_SCANCODE_H: {
      return alloy::platform::keyboard::scan_code::letter_h;
    }
    case SDL_SCANCODE_I: {
      return alloy::platform::keyboard::scan_code::letter_i;
    }
    case SDL_SCANCODE_J: {
      return alloy::platform::keyboard::scan_code::letter_j;
    }
    case SDL_SCANCODE_K: {
      return alloy::platform::keyboard::scan_code::letter_k;
    }
    case SDL_SCANCODE_L: {
      return alloy::platform::keyboard::scan_code::letter_l;
    }
    case SDL_SCANCODE_M: {
      return alloy::platform::keyboard::scan_code::letter_m;
    }
    case SDL_SCANCODE_N: {
      return alloy::platform::keyboard::scan_code::letter_n;
    }
    case SDL_SCANCODE_O: {
      return alloy::platform::keyboard::scan_code::letter_o;
    }
    case SDL_SCANCODE_P: {
      return alloy::platform::keyboard::scan_code::letter_p;
    }
    case SDL_SCANCODE_Q: {
      return alloy::platform::keyboard::scan_code::letter_q;
    }
    case SDL_SCANCODE_R: {
      return alloy::platform::keyboard::scan_code::letter_r;
    }
    case SDL_SCANCODE_S: {
      return alloy::platform::keyboard::scan_code::letter_s;
    }
    case SDL_SCANCODE_T: {
      return alloy::platform::keyboard::scan_code::letter_t;
    }
    case SDL_SCANCODE_U: {
      return alloy::platform::keyboard::scan_code::letter_u;
    }
    case SDL_SCANCODE_V: {
      return alloy::platform::keyboard::scan_code::letter_v;
    }
    case SDL_SCANCODE_W: {
      return alloy::platform::keyboard::scan_code::letter_w;
    }
    case SDL_SCANCODE_X: {
      return alloy::platform::keyboard::scan_code::letter_x;
    }
    case SDL_SCANCODE_Y: {
      return alloy::platform::keyboard::scan_code::letter_y;
    }
    case SDL_SCANCODE_Z: {
      return alloy::platform::keyboard::scan_code::letter_z;
    }

    //-------------------------------------------------------------------------
    // Special
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_ESCAPE: {
      return alloy::platform::keyboard::scan_code::escape;
    }
    case SDL_SCANCODE_RETURN: {
      return alloy::platform::keyboard::scan_code::enter;
    }
    case SDL_SCANCODE_BACKSPACE: {
      return alloy::platform::keyboard::scan_code::backspace;
    }
    case SDL_SCANCODE_DELETE: {
      return alloy::platform::keyboard::scan_code::del;
    }
    case SDL_SCANCODE_INSERT: {
      return alloy::platform::keyboard::scan_code::insert;
    }
    case SDL_SCANCODE_PAUSE: {
      return alloy::platform::keyboard::scan_code::pause;
    }
    case SDL_SCANCODE_PRINTSCREEN: {
      return alloy::platform::keyboard::scan_code::print_screen;
    }

    //-------------------------------------------------------------------------
    // Glyphs
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_SPACE: {
      return alloy::platform::keyboard::scan_code::space;
    }
    case SDL_SCANCODE_TAB: {
      return alloy::platform::keyboard::scan_code::tab;
    }
    case SDL_SCANCODE_BACKSLASH: {
      return alloy::platform::keyboard::scan_code::backslash;
    }
    case SDL_SCANCODE_MINUS: {
      return alloy::platform::keyboard::scan_code::minus;
    }
    case SDL_SCANCODE_EQUALS: {
      return alloy::platform::keyboard::scan_code::equals;
    }
    case SDL_SCANCODE_LEFTBRACKET: {
      return alloy::platform::keyboard::scan_code::left_bracket;
    }
    case SDL_SCANCODE_RIGHTBRACKET: {
      return alloy::platform::keyboard::scan_code::right_bracket;
    }
    case SDL_SCANCODE_SEMICOLON: {
      return alloy::platform::keyboard::scan_code::semicolon;
    }
    case SDL_SCANCODE_APOSTROPHE: {
      return alloy::platform::keyboard::scan_code::apostrophe;
    }
    case SDL_SCANCODE_COMMA: {
      return alloy::platform::keyboard::scan_code::comma;
    }
    case SDL_SCANCODE_PERIOD: {
      return alloy::platform::keyboard::scan_code::period;
    }
    case SDL_SCANCODE_SLASH: {
      return alloy::platform::keyboard::scan_code::slash;
    }

    //-------------------------------------------------------------------------
    // Control
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_LSHIFT: {
      return alloy::platform::keyboard::scan_code::left_shift;
    }
    case SDL_SCANCODE_RSHIFT: {
      return alloy::platform::keyboard::scan_code::right_shift;
    }
    case SDL_SCANCODE_LALT: {
      return alloy::platform::keyboard::scan_code::left_alt;
    }
    case SDL_SCANCODE_RALT: {
      return alloy::platform::keyboard::scan_code::right_alt;
    }
    case SDL_SCANCODE_LGUI: {
      return alloy::platform::keyboard::scan_code::left_host;
    }
    case SDL_SCANCODE_RGUI: {
      return alloy::platform::keyboard::scan_code::right_host;
    }
    case SDL_SCANCODE_LCTRL: {
      return alloy::platform::keyboard::scan_code::left_control;
    }
    case SDL_SCANCODE_RCTRL: {
      return alloy::platform::keyboard::scan_code::right_control;
    }
    case SDL_SCANCODE_MENU: {
      return alloy::platform::keyboard::scan_code::menu;
    }

    //-------------------------------------------------------------------------
    // Locking
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_CAPSLOCK: {
      return alloy::platform::keyboard::scan_code::caps_lock;
    }
    case SDL_SCANCODE_NUMLOCKCLEAR: {
      return alloy::platform::keyboard::scan_code::num_lock;
    }

    //-------------------------------------------------------------------------
    // Navigation
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_UP: {
      return alloy::platform::keyboard::scan_code::arrow_up;
    }
    case SDL_SCANCODE_LEFT: {
      return alloy::platform::keyboard::scan_code::arrow_left;
    }
    case SDL_SCANCODE_DOWN: {
      return alloy::platform::keyboard::scan_code::arrow_down;
    }
    case SDL_SCANCODE_RIGHT: {
      return alloy::platform::keyboard::scan_code::arrow_right;
    }
    case SDL_SCANCODE_HOME: {
      return alloy::platform::keyboard::scan_code::home;
    }
    case SDL_SCANCODE_PAGEUP: {
      return alloy::platform::keyboard::scan_code::page_up;
    }
    case SDL_SCANCODE_PAGEDOWN: {
      return alloy::platform::keyboard::scan_code::page_down;
    }

    //-------------------------------------------------------------------------
    // Keypad
    //-------------------------------------------------------------------------

    case SDL_SCANCODE_KP_000: {
      return alloy::platform::keyboard::scan_code::keypad_num_000;
    }
    case SDL_SCANCODE_KP_00: {
      return alloy::platform::keyboard::scan_code::keypad_num_00;
    }
    case SDL_SCANCODE_KP_0: {
      return alloy::platform::keyboard::scan_code::keypad_num_0;
    }
    case SDL_SCANCODE_KP_1: {
      return alloy::platform::keyboard::scan_code::keypad_num_1;
    }
    case SDL_SCANCODE_KP_2: {
      return alloy::platform::keyboard::scan_code::keypad_num_2;
    }
    case SDL_SCANCODE_KP_3: {
      return alloy::platform::keyboard::scan_code::keypad_num_3;
    }
    case SDL_SCANCODE_KP_4: {
      return alloy::platform::keyboard::scan_code::keypad_num_4;
    }
    case SDL_SCANCODE_KP_5: {
      return alloy::platform::keyboard::scan_code::keypad_num_5;
    }
    case SDL_SCANCODE_KP_6: {
      return alloy::platform::keyboard::scan_code::keypad_num_6;
    }
    case SDL_SCANCODE_KP_7: {
      return alloy::platform::keyboard::scan_code::keypad_num_7;
    }
    case SDL_SCANCODE_KP_8: {
      return alloy::platform::keyboard::scan_code::keypad_num_8;
    }
    case SDL_SCANCODE_KP_9: {
      return alloy::platform::keyboard::scan_code::keypad_num_9;
    }
    case SDL_SCANCODE_KP_ENTER: {
      return alloy::platform::keyboard::scan_code::keypad_enter;
    }
    case SDL_SCANCODE_KP_PERIOD: {
      return alloy::platform::keyboard::scan_code::keypad_period;
    }
    case SDL_SCANCODE_KP_PLUS: {
      return alloy::platform::keyboard::scan_code::keypad_plus;
    }
    case SDL_SCANCODE_KP_MINUS: {
      return alloy::platform::keyboard::scan_code::keypad_minus;
    }
    case SDL_SCANCODE_KP_MULTIPLY: {
      return alloy::platform::keyboard::scan_code::keypad_multiply;
    }
    case SDL_SCANCODE_KP_DIVIDE: {
      return alloy::platform::keyboard::scan_code::keypad_divide;
    }

    default: break;
  }

  return alloy::platform::keyboard::scan_code::unknown;
}

auto to_key_code(::SDL_Keycode code) -> alloy::platform::keyboard::key_code
{
  switch (static_cast<::SDL_Keycode>(code)) {
    //-------------------------------------------------------------------------
    // Function Keys
    //-------------------------------------------------------------------------

    case SDLK_F1: {
      return alloy::platform::keyboard::key_code::f1;
    }
    case SDLK_F2: {
      return alloy::platform::keyboard::key_code::f2;
    }
    case SDLK_F3: {
      return alloy::platform::keyboard::key_code::f3;
    }
    case SDLK_F4: {
      return alloy::platform::keyboard::key_code::f4;
    }
    case SDLK_F5: {
      return alloy::platform::keyboard::key_code::f5;
    }
    case SDLK_F6: {
      return alloy::platform::keyboard::key_code::f6;
    }
    case SDLK_F7: {
      return alloy::platform::keyboard::key_code::f7;
    }
    case SDLK_F8: {
      return alloy::platform::keyboard::key_code::f8;
    }
    case SDLK_F9: {
      return alloy::platform::keyboard::key_code::f9;
    }
    case SDLK_F10: {
      return alloy::platform::keyboard::key_code::f10;
    }
    case SDLK_F11: {
      return alloy::platform::keyboard::key_code::f11;
    }
    case SDLK_F12: {
      return alloy::platform::keyboard::key_code::f12;
    }
    case SDLK_F13: {
      return alloy::platform::keyboard::key_code::f13;
    }
    case SDLK_F14: {
      return alloy::platform::keyboard::key_code::f14;
    }
    case SDLK_F15: {
      return alloy::platform::keyboard::key_code::f15;
    }
    case SDLK_F16: {
      return alloy::platform::keyboard::key_code::f16;
    }
    case SDLK_F17: {
      return alloy::platform::keyboard::key_code::f17;
    }
    case SDLK_F18: {
      return alloy::platform::keyboard::key_code::f18;
    }
    case SDLK_F19: {
      return alloy::platform::keyboard::key_code::f19;
    }
    case SDLK_F20: {
      return alloy::platform::keyboard::key_code::f20;
    }
    case SDLK_F21: {
      return alloy::platform::keyboard::key_code::f21;
    }
    case SDLK_F22: {
      return alloy::platform::keyboard::key_code::f22;
    }
    case SDLK_F23: {
      return alloy::platform::keyboard::key_code::f23;
    }
    case SDLK_F24: {
      return alloy::platform::keyboard::key_code::f24;
    }

    //-------------------------------------------------------------------------
    // Number row
    //-------------------------------------------------------------------------

    case SDLK_0: {
      return alloy::platform::keyboard::key_code::number_0;
    }
    case SDLK_1: {
      return alloy::platform::keyboard::key_code::number_1;
    }
    case SDLK_2: {
      return alloy::platform::keyboard::key_code::number_2;
    }
    case SDLK_3: {
      return alloy::platform::keyboard::key_code::number_3;
    }
    case SDLK_4: {
      return alloy::platform::keyboard::key_code::number_4;
    }
    case SDLK_5: {
      return alloy::platform::keyboard::key_code::number_5;
    }
    case SDLK_6: {
      return alloy::platform::keyboard::key_code::number_6;
    }
    case SDLK_7: {
      return alloy::platform::keyboard::key_code::number_7;
    }
    case SDLK_8: {
      return alloy::platform::keyboard::key_code::number_8;
    }
    case SDLK_9: {
      return alloy::platform::keyboard::key_code::number_9;
    }

    //-------------------------------------------------------------------------
    // Letters
    //-------------------------------------------------------------------------

    case SDLK_A: {
      return alloy::platform::keyboard::key_code::letter_a;
    }
    case SDLK_B: {
      return alloy::platform::keyboard::key_code::letter_b;
    }
    case SDLK_C: {
      return alloy::platform::keyboard::key_code::letter_c;
    }
    case SDLK_D: {
      return alloy::platform::keyboard::key_code::letter_d;
    }
    case SDLK_E: {
      return alloy::platform::keyboard::key_code::letter_e;
    }
    case SDLK_F: {
      return alloy::platform::keyboard::key_code::letter_f;
    }
    case SDLK_G: {
      return alloy::platform::keyboard::key_code::letter_g;
    }
    case SDLK_H: {
      return alloy::platform::keyboard::key_code::letter_h;
    }
    case SDLK_I: {
      return alloy::platform::keyboard::key_code::letter_i;
    }
    case SDLK_J: {
      return alloy::platform::keyboard::key_code::letter_j;
    }
    case SDLK_K: {
      return alloy::platform::keyboard::key_code::letter_k;
    }
    case SDLK_L: {
      return alloy::platform::keyboard::key_code::letter_l;
    }
    case SDLK_M: {
      return alloy::platform::keyboard::key_code::letter_m;
    }
    case SDLK_N: {
      return alloy::platform::keyboard::key_code::letter_n;
    }
    case SDLK_O: {
      return alloy::platform::keyboard::key_code::letter_o;
    }
    case SDLK_P: {
      return alloy::platform::keyboard::key_code::letter_p;
    }
    case SDLK_Q: {
      return alloy::platform::keyboard::key_code::letter_q;
    }
    case SDLK_R: {
      return alloy::platform::keyboard::key_code::letter_r;
    }
    case SDLK_S: {
      return alloy::platform::keyboard::key_code::letter_s;
    }
    case SDLK_T: {
      return alloy::platform::keyboard::key_code::letter_t;
    }
    case SDLK_U: {
      return alloy::platform::keyboard::key_code::letter_u;
    }
    case SDLK_V: {
      return alloy::platform::keyboard::key_code::letter_v;
    }
    case SDLK_W: {
      return alloy::platform::keyboard::key_code::letter_w;
    }
    case SDLK_X: {
      return alloy::platform::keyboard::key_code::letter_x;
    }
    case SDLK_Y: {
      return alloy::platform::keyboard::key_code::letter_y;
    }
    case SDLK_Z: {
      return alloy::platform::keyboard::key_code::letter_z;
    }

    //-------------------------------------------------------------------------
    // Special
    //-------------------------------------------------------------------------

    case SDLK_ESCAPE: {
      return alloy::platform::keyboard::key_code::escape;
    }
    case SDLK_RETURN: {
      return alloy::platform::keyboard::key_code::enter;
    }
    case SDLK_BACKSPACE: {
      return alloy::platform::keyboard::key_code::backspace;
    }
    case SDLK_DELETE: {
      return alloy::platform::keyboard::key_code::del;
    }
    case SDLK_INSERT: {
      return alloy::platform::keyboard::key_code::insert;
    }
    case SDLK_PAUSE: {
      return alloy::platform::keyboard::key_code::pause;
    }
    case SDLK_PRINTSCREEN: {
      return alloy::platform::keyboard::key_code::print_screen;
    }

    //-------------------------------------------------------------------------
    // Glyphs
    //-------------------------------------------------------------------------

    case SDLK_SPACE: {
      return alloy::platform::keyboard::key_code::space;
    }
    case SDLK_TAB: {
      return alloy::platform::keyboard::key_code::tab;
    }
    case SDLK_BACKSLASH: {
      return alloy::platform::keyboard::key_code::backslash;
    }
    case SDLK_MINUS: {
      return alloy::platform::keyboard::key_code::minus;
    }
    case SDLK_EQUALS: {
      return alloy::platform::keyboard::key_code::equals;
    }
    case SDLK_LEFTBRACKET: {
      return alloy::platform::keyboard::key_code::left_bracket;
    }
    case SDLK_RIGHTBRACKET: {
      return alloy::platform::keyboard::key_code::right_bracket;
    }
    case SDLK_SEMICOLON: {
      return alloy::platform::keyboard::key_code::semicolon;
    }
    case SDLK_APOSTROPHE: {
      return alloy::platform::keyboard::key_code::apostrophe;
    }
    case SDLK_COMMA: {
      return alloy::platform::keyboard::key_code::comma;
    }
    case SDLK_PERIOD: {
      return alloy::platform::keyboard::key_code::period;
    }
    case SDLK_SLASH: {
      return alloy::platform::keyboard::key_code::slash;
    }

    //-------------------------------------------------------------------------
    // Control
    //-------------------------------------------------------------------------

    case SDLK_LSHIFT: {
      return alloy::platform::keyboard::key_code::left_shift;
    }
    case SDLK_RSHIFT: {
      return alloy::platform::keyboard::key_code::right_shift;
    }
    case SDLK_LALT: {
      return alloy::platform::keyboard::key_code::left_alt;
    }
    case SDLK_RALT: {
      return alloy::platform::keyboard::key_code::right_alt;
    }
    case SDLK_LGUI: {
      return alloy::platform::keyboard::key_code::left_host;
    }
    case SDLK_RGUI: {
      return alloy::platform::keyboard::key_code::right_host;
    }
    case SDLK_LCTRL: {
      return alloy::platform::keyboard::key_code::left_control;
    }
    case SDLK_RCTRL: {
      return alloy::platform::keyboard::key_code::right_control;
    }
    case SDLK_MENU: {
      return alloy::platform::keyboard::key_code::menu;
    }

    //-------------------------------------------------------------------------
    // Locking
    //-------------------------------------------------------------------------

    case SDLK_CAPSLOCK: {
      return alloy::platform::keyboard::key_code::caps_lock;
    }
    case SDLK_NUMLOCKCLEAR: {
      return alloy::platform::keyboard::key_code::num_lock;
    }


    //-------------------------------------------------------------------------
    // Navigation
    //-------------------------------------------------------------------------

    case SDLK_UP: {
      return alloy::platform::keyboard::key_code::arrow_up;
    }
    case SDLK_LEFT: {
      return alloy::platform::keyboard::key_code::arrow_left;
    }
    case SDLK_DOWN: {
      return alloy::platform::keyboard::key_code::arrow_down;
    }
    case SDLK_RIGHT: {
      return alloy::platform::keyboard::key_code::arrow_right;
    }
    case SDLK_HOME: {
      return alloy::platform::keyboard::key_code::home;
    }
    case SDLK_PAGEUP: {
      return alloy::platform::keyboard::key_code::page_up;
    }
    case SDLK_PAGEDOWN: {
      return alloy::platform::keyboard::key_code::page_down;
    }

    //-------------------------------------------------------------------------
    // Keypad
    //-------------------------------------------------------------------------

    case SDLK_KP_000: {
      return alloy::platform::keyboard::key_code::keypad_num_000;
    }
    case SDLK_KP_00: {
      return alloy::platform::keyboard::key_code::keypad_num_00;
    }
    case SDLK_KP_0: {
      return alloy::platform::keyboard::key_code::keypad_num_0;
    }
    case SDLK_KP_1: {
      return alloy::platform::keyboard::key_code::keypad_num_1;
    }
    case SDLK_KP_2: {
      return alloy::platform::keyboard::key_code::keypad_num_2;
    }
    case SDLK_KP_3: {
      return alloy::platform::keyboard::key_code::keypad_num_3;
    }
    case SDLK_KP_4: {
      return alloy::platform::keyboard::key_code::keypad_num_4;
    }
    case SDLK_KP_5: {
      return alloy::platform::keyboard::key_code::keypad_num_5;
    }
    case SDLK_KP_6: {
      return alloy::platform::keyboard::key_code::keypad_num_6;
    }
    case SDLK_KP_7: {
      return alloy::platform::keyboard::key_code::keypad_num_7;
    }
    case SDLK_KP_8: {
      return alloy::platform::keyboard::key_code::keypad_num_8;
    }
    case SDLK_KP_9: {
      return alloy::platform::keyboard::key_code::keypad_num_9;
    }
    case SDLK_KP_ENTER: {
      return alloy::platform::keyboard::key_code::keypad_enter;
    }
    case SDLK_KP_PERIOD: {
      return alloy::platform::keyboard::key_code::keypad_period;
    }
    case SDLK_KP_PLUS: {
      return alloy::platform::keyboard::key_code::keypad_plus;
    }
    case SDLK_KP_MINUS: {
      return alloy::platform::keyboard::key_code::keypad_minus;
    }
    case SDLK_KP_MULTIPLY: {
      return alloy::platform::keyboard::key_code::keypad_multiply;
    }
    case SDLK_KP_DIVIDE: {
      return alloy::platform::keyboard::key_code::keypad_divide;
    }

    default: break;
  }

  return alloy::platform::keyboard::key_code::unknown;
}

auto to_modifiers(::SDL_Keymod modifiers) -> alloy::platform::keyboard::modifiers
{
  const auto mod = modifiers;

  auto result = alloy::platform::keyboard::modifiers::none;

  if (mod & SDL_KMOD_LSHIFT) {
    result |= alloy::platform::keyboard::modifiers::left_shift;
  }
  if (mod & SDL_KMOD_RSHIFT) {
    result |= alloy::platform::keyboard::modifiers::right_shift;
  }

  if (mod & SDL_KMOD_LCTRL) {
    result |= alloy::platform::keyboard::modifiers::left_control;
  }
  if (mod & SDL_KMOD_RCTRL) {
    result |= alloy::platform::keyboard::modifiers::right_control;
  }

  if (mod & SDL_KMOD_LALT) {
    result |= alloy::platform::keyboard::modifiers::left_alt;
  }
  if (mod & SDL_KMOD_RALT) {
    result |= alloy::platform::keyboard::modifiers::right_alt;
  }

  if (mod & SDL_KMOD_LGUI) {
    result |= alloy::platform::keyboard::modifiers::left_host;
  }
  if (mod & SDL_KMOD_RGUI) {
    result |= alloy::platform::keyboard::modifiers::right_host;
  }

  if (mod & SDL_KMOD_NUM) {
    result |= alloy::platform::keyboard::modifiers::num_lock;
  }
  if (mod & SDL_KMOD_CAPS) {
    result |= alloy::platform::keyboard::modifiers::caps_lock;
  }
  if (mod & SDL_KMOD_MODE) {
    result |= alloy::platform::keyboard::modifiers::mode;
  }

  return result;
}

} // namespace <anonymous>

//------------------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_pump_source::instance() -> sdl2_pump_source&
{
  static auto s_instance = sdl2_pump_source{};

  return s_instance;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_pump_source::attach_window(
  core::not_null<sdl2_window*> window
) -> void
{
  const auto id = ::SDL_GetWindowID(window->window_handle());
  m_windows[static_cast<unsigned>(id)] = window.get();
}

auto alloy::extra::sdl2_pump_source::detach_window(
  core::not_null<sdl2_window*> window
) -> void
{
  const auto id = ::SDL_GetWindowID(window->window_handle());
  const auto it = m_windows.find(static_cast<unsigned>(id));

  ALLOY_ASSERT( it != m_windows.end() );

  m_windows.erase(it);
}

//------------------------------------------------------------------------------
// Hooks
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_pump_source::poll(platform::message_pump& p) noexcept -> void
{
  auto event = ::SDL_Event{};

  // Poll all events and translate them
  while (::SDL_PollEvent(&event)) {
    if (is_window_event(event)) {
      handle_window_event(event, p);
      continue;
    }
    switch (event.type) {
      case SDL_EVENT_KEY_UP: {
        handle_keyboard_event(event, platform::keyboard::key_state::depressed, p);
        break;
      }
      case SDL_EVENT_KEY_DOWN: {
        handle_keyboard_event(event, platform::keyboard::key_state::pressed, p);
        break;
      }
      case SDL_EVENT_MOUSE_WHEEL:
      case SDL_EVENT_MOUSE_MOTION:
      case SDL_EVENT_MOUSE_BUTTON_UP:
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        handle_mouse_event(event, p);
        break;
      }
      default: {
        // Events that have no message_pump equivalent are ignored.
        break;
      }
    }
  }
}

auto alloy::extra::sdl2_pump_source::handle_window_event(
  const ::SDL_Event& event,
  platform::message_pump& p
) noexcept -> void
{
  const auto id = event.window.windowID;
  const auto it = m_windows.find(id);

  ALLOY_ASSERT(is_window_event(event));

  // Ignore windows that aren't attached; no reason to fire these events.
  if (it == m_windows.end()) {
    return;
  }

  const auto window = it->second;

  switch (event.type) {

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
      // Close events should be listened to as soon as possible;
      // so this is posted as a high-priority event
      p.post_immediate_event( platform::window_close_event{window} );
      break;
    }

    case SDL_EVENT_WINDOW_MINIMIZED: {
      p.post_event( platform::window_minimize_event{window} );
      break;
    }
    case SDL_EVENT_WINDOW_MAXIMIZED: {
      p.post_event( platform::window_maximize_event{window} );
      break;
    }
    case SDL_EVENT_WINDOW_RESTORED: {
      p.post_event( platform::window_restore_event{window} );
      break;
    }

    case SDL_EVENT_WINDOW_HIDDEN: {
      p.post_event( platform::window_hide_event{window} );
      break;
    }
    case SDL_EVENT_WINDOW_EXPOSED: {
      // TODO(bitwizeshift):
      //   only fire this event if window was hidden before this change
      p.post_event( platform::window_show_event{window} );
      break;
    }

    case SDL_EVENT_WINDOW_MOVED: {
      const auto x = event.window.data1;
      const auto y = event.window.data2;

      p.post_event( platform::window_move_event{window, x, y} );
      break;
    }
    case SDL_EVENT_WINDOW_RESIZED: {
      const auto width = event.window.data1;
      const auto height = event.window.data2;

      p.post_event( platform::window_resize_event{window, width, height} );
      break;
    }

    case SDL_EVENT_WINDOW_FOCUS_GAINED: {
      p.post_event( platform::window_focus_event{window} );
      break;
    }
    case SDL_EVENT_WINDOW_FOCUS_LOST: {
      p.post_event( platform::window_unfocus_event{window} );
      break;
    }
    default: {
      break;
    }
  }
}

auto alloy::extra::sdl2_pump_source::handle_keyboard_event(
  const ::SDL_Event& event,
  platform::keyboard::key_state state,
  platform::message_pump& p
) noexcept -> void
{
  const auto id = event.window.windowID;
  const auto it = m_windows.find(id);

  ALLOY_ASSERT(event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_KEY_DOWN);

  // Ignore windows that aren't attached; no reason to fire these events.
  if (it == m_windows.end()) {
    return;
  }

  const auto window = it->second;

  const auto scan_code = ::to_scan_code(event.key.scancode);
  const auto key_code  = ::to_key_code(event.key.key);
  const auto modifiers = ::to_modifiers(event.key.mod);

  p.post_event(platform::keyboard_event{
    window,
    platform::keyboard::symbol{ scan_code, key_code, modifiers, },
    state
  });
}

auto alloy::extra::sdl2_pump_source::handle_mouse_event(
  const SDL_Event& event,
  alloy::platform::message_pump& p
) noexcept -> void
{
  const auto id = event.window.windowID;
  const auto it = m_windows.find(id);

  // Ignore windows that aren't attached; no reason to fire these events.
  if (it == m_windows.end()) {
    return;
  }

  const auto window = it->second;

  switch (event.type) {
    case SDL_EVENT_MOUSE_WHEEL: {
      p.post_event(platform::mouse_scroll_event{
        window,
        event.wheel.integer_x, event.wheel.integer_y
      });
      return;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      p.post_event(platform::mouse_move_event{
        window,
        static_cast<std::int32_t>(event.motion.x),
        static_cast<std::int32_t>(event.motion.y),
        static_cast<std::int32_t>(event.motion.xrel),
        static_cast<std::int32_t>(event.motion.yrel),
      });
      return;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: [[fallthrough]];
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      const auto button = to_mouse_button(event.button.button);
      const auto state = event.button.down
        ? platform::mouse::button_state::pressed
        : platform::mouse::button_state::depressed;

      if (!button) { return; }

      p.post_event(platform::mouse_button_event{
        window,
        static_cast<std::int32_t>(event.button.x),
        static_cast<std::int32_t>(event.button.y),
        *button, state
      });
      return;
    }
    default: {
      break;
    }
  }
}
