////////////////////////////////////////////////////////////////////////////////
/// \file keyboard.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ALLOY_IO_KEYBOARD_HPP
#define ALLOY_IO_KEYBOARD_HPP

#include <cstdint>     // std::uint8_t, std::uint16_t, etc
#include <type_traits> // std::underlying_type_t

namespace alloy::io {

  //===========================================================================
  // struct : keyboard
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A type holder for various keyboard types
  /////////////////////////////////////////////////////////////////////////////
  struct keyboard
  {
    enum class scan_code : std::uint16_t;
    enum class key_code : std::uint16_t;
    enum class modifiers : std::uint16_t;
    enum class key_state : std::uint8_t;
    struct symbol;
  };

  //===========================================================================
  // enum class : keyboard::scan_code
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This represents the physical position of the key on the keyboard
  ///
  /// This is modeled after the typical QWERTY keyboard. This allows for
  /// an AZERTY keyboard to register an 'a' keypress as a 'q' scan_code.
  ///
  /// To see the actual key that is pressed, see keyboard::key_code
  /////////////////////////////////////////////////////////////////////////////
  enum class keyboard::scan_code : std::uint16_t
  {
    unknown,

    //-------------------------------------------------------------------------
    // Function keys
    //-------------------------------------------------------------------------

    f1,  f2,  f3,  f4,  f5,  f6,  f7,  f8,  f9,  f10, f11, f12,
    f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,

    //-------------------------------------------------------------------------
    // Number row
    //-------------------------------------------------------------------------

    number_0, number_1, number_2, number_3, number_4, number_5, number_6,
    number_7, number_8, number_9,

    //-------------------------------------------------------------------------
    // Letters
    //-------------------------------------------------------------------------

    letter_a, letter_b, letter_c, letter_d, letter_e, letter_f, letter_g,
    letter_h, letter_i, letter_j, letter_k, letter_l, letter_m, letter_n,
    letter_o, letter_p, letter_q, letter_r, letter_s, letter_t, letter_u,
    letter_v, letter_w, letter_x, letter_y, letter_z,

    //-------------------------------------------------------------------------
    // Special
    //-------------------------------------------------------------------------

    escape, enter,
    backspace, del, insert,
    pause, print_screen,

    //-------------------------------------------------------------------------
    // Glyphs
    //-------------------------------------------------------------------------

    space, tab, backslash,
    minus, equals, left_bracket, right_bracket,
    semicolon, apostrophe, comma, period, slash,

    //-------------------------------------------------------------------------
    // Control
    //-------------------------------------------------------------------------

    left_shift, right_shift,
    left_alt, right_alt,
    left_host, right_host,
    left_control, right_control,
    menu,

    //-------------------------------------------------------------------------
    // Locking
    //-------------------------------------------------------------------------

    num_lock, caps_lock,

    //-------------------------------------------------------------------------
    // Navigation
    //-------------------------------------------------------------------------

    arrow_up, arrow_left, arrow_down, arrow_right,
    home, page_up, page_down,

    //-------------------------------------------------------------------------
    // Keypad
    //-------------------------------------------------------------------------

    keypad_num_000, keypad_num_00, keypad_num_0,
    keypad_num_1,   keypad_num_2,  keypad_num_3,
    keypad_num_4,   keypad_num_5,  keypad_num_6,
    keypad_num_7,   keypad_num_8,  keypad_num_9,
    keypad_enter,   keypad_period,
    keypad_plus,    keypad_minus,  keypad_multiply,  keypad_divide,
  };

  //===========================================================================
  // enum class : keyboard::key_code
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The actual key value that was pressed, as given by the driver.
  ///
  /// Unlike the scan_code, the key_code will always represent the exact key
  /// that is pressed. Thus, on an AZERTY keyboard, pressing 'a' will yield an
  /// 'a' key_code.
  ///
  /// In general, the scan_code is often more useful for game input, where the
  /// key_code may be more useful for keyboard input.
  /////////////////////////////////////////////////////////////////////////////
  enum class keyboard::key_code : std::uint16_t
  {
    unknown,

    //-------------------------------------------------------------------------
    // Function keys
    //-------------------------------------------------------------------------

    f1,  f2,  f3,  f4,  f5,  f6,  f7,  f8,  f9,  f10, f11, f12,
    f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,

    //-------------------------------------------------------------------------
    // Number row
    //-------------------------------------------------------------------------

    number_0, number_1, number_2, number_3, number_4, number_5, number_6,
    number_7, number_8, number_9,

    //-------------------------------------------------------------------------
    // Letters
    //-------------------------------------------------------------------------

    letter_a, letter_b, letter_c, letter_d, letter_e, letter_f, letter_g,
    letter_h, letter_i, letter_j, letter_k, letter_l, letter_m, letter_n,
    letter_o, letter_p, letter_q, letter_r, letter_s, letter_t, letter_u,
    letter_v, letter_w, letter_x, letter_y, letter_z,

    //-------------------------------------------------------------------------
    // Special
    //-------------------------------------------------------------------------

    escape, enter,
    backspace, del, insert,
    pause, print_screen,

    //-------------------------------------------------------------------------
    // Glyphs
    //-------------------------------------------------------------------------

    space, tab, backslash,
    minus, equals, left_bracket, right_bracket,
    semicolon, apostrophe, comma, period, slash,

    //-------------------------------------------------------------------------
    // Control
    //-------------------------------------------------------------------------

    left_shift, right_shift,
    left_alt, right_alt,
    left_host, right_host,
    left_control, right_control,
    menu,

    //-------------------------------------------------------------------------
    // Locking
    //-------------------------------------------------------------------------

    num_lock, caps_lock,

    //-------------------------------------------------------------------------
    // Navigation
    //-------------------------------------------------------------------------

    arrow_up, arrow_left, arrow_down, arrow_right,
    home, page_up, page_down,

    //-------------------------------------------------------------------------
    // Keypad
    //-------------------------------------------------------------------------

    keypad_num_000, keypad_num_00, keypad_num_0,
    keypad_num_1,   keypad_num_2,  keypad_num_3,
    keypad_num_4,   keypad_num_5,  keypad_num_6,
    keypad_num_7,   keypad_num_8,  keypad_num_9,
    keypad_enter,   keypad_period,
    keypad_plus,    keypad_minus,  keypad_multiply,  keypad_divide,
  };

  //===========================================================================
  // enum class : keyboard::modifiers
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Modifiers that may affect the keys.
  ///
  /// The modifiers are the conventional shift, alt, control (ctrl), host
  /// ("windows key" or "command"), and the various "lock" keys like caps lock
  /// and num lock.
  /////////////////////////////////////////////////////////////////////////////
  enum class keyboard::modifiers : std::uint16_t
  {
    none   = (0u << 0u),

    // Shift
    left_shift = (1u << 0u),
    right_shift = (1u << 1u),

    // Control
    left_control = (1u << 2u),
    right_control = (1u << 3u),

    // Alt
    left_alt = (1u << 4u),
    right_alt = (1u << 5u),

    // Host
    left_host = (1u << 6u),
    right_host = (1u << 7u),

    // Other
    caps_lock = (1u << 8u),
    num_lock  = (1u << 9u),
    mode      = (1u << 10u),
  };

  //===========================================================================
  // non-member functions : enum class : keyboard::scan_code
  //===========================================================================

  //---------------------------------------------------------------------------
  // Bitwise Operators
  //---------------------------------------------------------------------------

  constexpr keyboard::modifiers
    operator|(keyboard::modifiers lhs, keyboard::modifiers rhs) noexcept;
  constexpr keyboard::modifiers
    operator^(keyboard::modifiers lhs, keyboard::modifiers rhs) noexcept;
  constexpr keyboard::modifiers
    operator&(keyboard::modifiers lhs, keyboard::modifiers rhs) noexcept;

  //---------------------------------------------------------------------------

  constexpr keyboard::modifiers&
    operator|=(keyboard::modifiers& lhs, keyboard::modifiers rhs) noexcept;
  constexpr keyboard::modifiers&
    operator^=(keyboard::modifiers& lhs, keyboard::modifiers rhs) noexcept;
  constexpr keyboard::modifiers&
    operator&=(keyboard::modifiers& lhs, keyboard::modifiers rhs) noexcept;

  //===========================================================================
  // enum class : keyboard::key_state
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The state of the key
  ///
  /// Keys may be in one of two states: pressed, or depressed (released)
  /////////////////////////////////////////////////////////////////////////////
  enum class keyboard::key_state : std::uint8_t
  {
    depressed, ///< Key is not pressed down
    pressed,   ///< Key is pressed down
  };

  //===========================================================================
  // class : keyboard::symbol
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A symbol state from the keyboard
  /////////////////////////////////////////////////////////////////////////////
  struct keyboard::symbol
  {
    keyboard::scan_code scan_code;
    keyboard::key_code  key_code;
    keyboard::modifiers modifiers;
  };

} // namespace alloy::io

//=============================================================================
// non-member functions : enum class : keyboard::scan_code
//=============================================================================

//-----------------------------------------------------------------------------
// Bitwise Operators
//-----------------------------------------------------------------------------

inline constexpr alloy::io::keyboard::modifiers
  alloy::io::operator|(keyboard::modifiers lhs, keyboard::modifiers rhs)
  noexcept
{
  using value_type = std::underlying_type_t<keyboard::modifiers>;

  return static_cast<keyboard::modifiers>(
    static_cast<value_type>(lhs) | static_cast<value_type>(rhs)
  );
}

inline constexpr alloy::io::keyboard::modifiers
  alloy::io::operator^(keyboard::modifiers lhs, keyboard::modifiers rhs)
  noexcept
{
  using value_type = std::underlying_type_t<keyboard::modifiers>;

  return static_cast<keyboard::modifiers>(
    static_cast<value_type>(lhs) ^ static_cast<value_type>(rhs)
  );

}

inline constexpr alloy::io::keyboard::modifiers
  alloy::io::operator&(keyboard::modifiers lhs, keyboard::modifiers rhs)
  noexcept
{
  using value_type = std::underlying_type_t<keyboard::modifiers>;

  return static_cast<keyboard::modifiers>(
    static_cast<value_type>(lhs) & static_cast<value_type>(rhs)
  );
}

//---------------------------------------------------------------------------

inline constexpr alloy::io::keyboard::modifiers&
  alloy::io::operator|=(keyboard::modifiers& lhs, keyboard::modifiers rhs)
  noexcept
{
  return lhs = (lhs | rhs);
}

inline constexpr alloy::io::keyboard::modifiers&
  alloy::io::operator^=(keyboard::modifiers& lhs, keyboard::modifiers rhs)
  noexcept
{
  return lhs = (lhs ^ rhs);
}

inline constexpr alloy::io::keyboard::modifiers&
  alloy::io::operator&=(keyboard::modifiers& lhs, keyboard::modifiers rhs)
  noexcept
{
  return lhs = (lhs & rhs);
}

#endif /* ALLOY_IO_KEYBOARD_HPP */
