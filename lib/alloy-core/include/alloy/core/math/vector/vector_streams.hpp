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

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR_OSTREAMS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR_OSTREAMS_HPP

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
auto alloy::core::operator<<(std::basic_ostream<CharT,Traits>& o,
                             const vector2& vec)
  -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector2{" << vec.x() << ", " << vec.y() << "}";
}

template <typename CharT, typename Traits>
inline
auto alloy::core::operator<<(std::basic_ostream<CharT,Traits>& o,
                             const vector3& vec)
  -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector3{" << vec.x() << ", " << vec.y() << ", " << vec.z() << "}";
}

template <typename CharT, typename Traits>
inline
auto alloy::core::operator<<(std::basic_ostream<CharT,Traits>& o,
                             const vector4& vec)
  -> std::basic_ostream<CharT,Traits>&
{
  return o << "vector4{" << vec.x() << ", " << vec.y() << ", "
                         << vec.z() << ", " << vec.w() << "}";
}

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR_STREAMS_HPP */
