// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file vector_streams.hpp
///
/// \brief This header contains the definition of output stream operators for
///        the vector class.
///
/// These are defined in this header rather than being in their respective
/// vector headers, since including "ostream" introduces overhead. This allows
/// a "pay-for-what-you-use" approach, since not all applications will ever
/// use ostream for the vector types other than perhaps for debugging
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR_STREAMS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR_STREAMS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"

#include <ostream>

namespace alloy::core {

  /// \{
  /// \brief Output stream operator for vector objects
  ///
  /// \param o reference to the output stream
  /// \param vec the vector to stream
  /// \return reference to the output stream
  template <typename CharT, typename Traits>
  auto operator<<(std::basic_ostream<CharT,Traits>& o, const vector2& vec)
    -> std::basic_ostream<CharT,Traits>&;
  template <typename CharT, typename Traits>
  auto operator<<(std::basic_ostream<CharT,Traits>& o, const vector3& vec)
    -> std::basic_ostream<CharT,Traits>&;
  template <typename CharT, typename Traits>
  auto operator<<(std::basic_ostream<CharT,Traits>& o, const vector4& vec)
    -> std::basic_ostream<CharT,Traits>&;
  /// \}

} // namespace alloy::core

template <typename CharT, typename Traits>
inline
auto alloy::core::operator<<(
  std::basic_ostream<CharT,Traits>& o,
  const vector2& vec
) -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector2{" << vec.x() << ", " << vec.y() << "}";
}

template <typename CharT, typename Traits>
inline
auto alloy::core::operator<<(
  std::basic_ostream<CharT,Traits>& o,
  const vector3& vec
) -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector3{" << vec.x() << ", " << vec.y() << ", " << vec.z() << "}";
}

template <typename CharT, typename Traits>
inline
auto alloy::core::operator<<(
  std::basic_ostream<CharT,Traits>& o,
  const vector4& vec
) -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector4{" << vec.x() << ", " << vec.y() << ", "
                         << vec.z() << ", " << vec.w() << "}";
}

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR_STREAMS_HPP */
