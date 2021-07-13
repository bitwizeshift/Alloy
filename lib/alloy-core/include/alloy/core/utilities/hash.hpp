////////////////////////////////////////////////////////////////////////////////
/// \file hash.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_UTILITIES_HASH_HPP
#define ALLOY_CORE_UTILITIES_HASH_HPP

#include "alloy/core/intrinsics.hpp" // ALLOY_FORCE_INLINE
#include "alloy/core/traits/for_each_member.hpp"

#include <functional>  // std::hash
#include <cstddef>     // std::size_t
#include <type_traits> // std::enable_if, std::is_enum_v, etc
#include <cstring>     // std::memcopy

namespace alloy::core {

  /// \brief A type that represents hash values
  using hash_type = decltype(sizeof(0));

  //----------------------------------------------------------------------------
  // Trait : STD hashable
  //----------------------------------------------------------------------------

  /// \brief A type-trait for detecting if `std::hash` was specialized for `T`
  template <typename T, typename = void>
  struct is_std_hashable : std::false_type {};

  template <typename T>
  struct is_std_hashable<T, std::void_t<decltype(
    std::hash<T>{}(std::declval<const T&>())
  )>> : std::true_type {};

  template <typename T>
  inline constexpr auto is_std_hashable_v = is_std_hashable<T>::value;

  //----------------------------------------------------------------------------
  // Hashing
  //----------------------------------------------------------------------------

  /// \brief Hashes a single value by using the underlying `std::hash`
  ///        implementation
  ///
  /// \param value the value to hash
  /// \return the hashed value
  template <typename T>
  constexpr auto hash_value(const T& value) noexcept -> hash_type;

  /// \brief Hashes an array of values
  ///
  /// \param array the array of values
  /// \return the hashed value
  template <typename T, unsigned N>
  constexpr auto hash_value(const T(&array)[N]) noexcept -> hash_type;

  /// \brief Hashes all specified \p values and joins them together
  ///
  /// \param value0 the first value in the sequence
  /// \param values the values to hash
  /// \return the hash
  template <typename T0, typename...Ts>
  constexpr auto hash_values(const T0& value0, const Ts&...values) noexcept -> hash_type;

  //----------------------------------------------------------------------------

  /// \brief Combines the hash \p seed with the hash of the \p value
  ///
  /// \param seed the seed for the hash to join
  /// \param value the value to hash
  /// \return the result of the joining
  template <typename T>
  constexpr auto hash_combine(hash_type seed, const T& value) noexcept -> hash_type;

  /// \brief Combines the hash \p seed with the hash of \p value0, \p value1,
  ///        and \p values
  ///
  /// \param seed the seed for the hash to join
  /// \param value0 the first value to hash
  /// \param value1 the second value to hash
  /// \param values the remaining values to hash
  /// \return the result of the joining
  template <typename T0, typename T1, typename...Ts>
  constexpr auto hash_combine(hash_type seed,
                              const T0& value0,
                              const T1& value1,
                              const Ts&...values)
    noexcept -> hash_type;

  //----------------------------------------------------------------------------

  /// \brief Hashes the range of entries in [first, last)
  ///
  /// \param first the start of the range
  /// \param last the end of the range
  /// \return the result of the hash
  template <typename It, typename End>
  constexpr auto hash_range(It first, End last) noexcept -> hash_type;

  /// \brief Combines the hash \p seed with the hash of the range of entries in
  ///        [first, last)
  ///
  /// \param seed the starting hash to join
  /// \param first the start of the range
  /// \param last the end of the range
  /// \return the result of the hash
  template <typename It, typename End>
  constexpr auto hash_combine_range(hash_type seed, It first, End last) noexcept -> hash_type;

  //----------------------------------------------------------------------------

  /// \brief Hashes an object by viewing its object representation
  ///
  /// The object being hashed must ave a unique object representation in order
  /// for this to operate correctly; otherwise there is no guarantee that two
  /// instances which compare equal may generate the same hash.
  ///
  /// \pre `has_unique_object_representation_v<T>` must be `true`
  ///
  /// \param value the value to hash
  /// \return the hashed value
  template <typename T>
  auto hash_object_representation(const T& value) noexcept -> hash_type;

  /// \brief Hashes an object by viewing its object representation and combines
  ///        it with \p seed
  ///
  /// The object being hashed must ave a unique object representation in order
  /// for this to operate correctly; otherwise there is no guarantee that two
  /// instances which compare equal may generate the same hash.
  ///
  /// \pre `has_unique_object_representation_v<T>` must be `true`
  ///
  /// \param value the value to hash
  /// \return the hashed value
  template <typename T>
  auto hash_combine_object_representation(std::size_t seed, const T& value) noexcept -> hash_type;

  //----------------------------------------------------------------------------

  /// \brief Hashes an aggregate by calling `hash_value` on each member of that
  ///        aggregate.
  ///
  /// \pre aggregate satisfies `std::is_aggregate_v`
  /// \param aggregate the aggregate to hash
  /// \return the hash value
  template <typename Aggregate>
  constexpr auto hash_aggregate(const Aggregate& aggregate) noexcept -> hash_type;

  /// \brief Hashes an aggregate by calling `hash_value` on each member of that
  ///        aggregate.
  ///
  /// \pre aggregate satisfies `std::is_aggregate_v`
  /// \param seed the seed to start the hash with
  /// \param aggregate the aggregate to hash
  /// \return the hash value
  template <typename Aggregate>
  constexpr auto hash_combine_aggregate(hash_type seed,
                                        const Aggregate& aggregate) noexcept -> hash_type;

  //----------------------------------------------------------------------------

  /// \brief ADL-calls hash_value
  ///
  /// \param value the value to hash
  /// \return the hashed value
  template <typename T>
  constexpr auto hash_object(const T& value) noexcept -> decltype(hash_value(value));

  //----------------------------------------------------------------------------

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A functor object that behaves similar to `std::hash`, only it
  ///        operates transparently by finding the `hash_object` overload
  ///
  /// This is meant to contrast `std::hash<T>` which requires unique
  /// specializations for each implementation. Rather, the overload approach
  /// allows for a simple way to ADL-discover new overloads, similar to `swap`.
  //////////////////////////////////////////////////////////////////////////////
  struct hash
  {
    template <typename T>
    constexpr auto operator()(const T& value) const noexcept -> decltype(hash_value(value));
  };

} // namespace alloy::core

//==============================================================================
// definitions : non-member functions
//==============================================================================

//------------------------------------------------------------------------------
// Hashing
//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash_value(const T& value)
  noexcept -> hash_type
{
  if constexpr (is_std_hashable_v<T>) {
    // If we have a std::hash implementation, use that

    return std::hash<T>{}(value);
  } else if constexpr (std::is_enum_v<T>) {
   // Convert to the underlying integer, then hash

    return hash_value(static_cast<std::underlying_type_t<T>>(value));
  } else if constexpr (std::is_aggregate_v<T>) {
    // Hash all aggregate members for us

    return hash_aggregate(value);
  } else if constexpr (std::has_unique_object_representations_v<T>) {
    // If an object has a unique object representation, try hashing those bits
    // by default. Note: this cannot be done at constexpr time without C++20's
    // std::bit_cast!

    return hash_object_representation(value);
  } else {
    static_assert(
      !is_std_hashable_v<T>,
      "No 'hash_value' overload discovered for specified type! "
      "Either implement an ADL-discoverable `hash_value` function for the type, "
      "or extend from `std::hash<T>`."
    );
  }
}

template <typename T, unsigned N>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash_value(const T(&array)[N])
  noexcept -> hash_type
{
  return hash_range(static_cast<const T*>(array), static_cast<const T*>(array) + N);
}

//------------------------------------------------------------------------------

template <typename T0, typename...Ts>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash_values(const T0& value0, const Ts&...values)
  noexcept -> hash_type
{
  return hash_combine(hash_type{0}, value0, values...);
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::hash_combine(hash_type seed, const T& value)
  noexcept -> hash_type
{
  // Implementation has been taken from boost::hash_combine
  return seed ^ (hash_value(value) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

template <typename T0, typename T1, typename...Ts>
inline constexpr
auto alloy::core::hash_combine(hash_type seed,
                               const T0& value0,
                               const T1& value1,
                               const Ts&...values)
  noexcept -> hash_type
{
  return hash_combine(hash_combine(seed, value0), value1, values...);
}

//------------------------------------------------------------------------------

template <typename It, typename End>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash_range(It first, End last)
  noexcept -> hash_type
{
  return hash_combine_range(hash_type{0}, first, last);
}

template <typename It, typename End>
inline constexpr
auto alloy::core::hash_combine_range(hash_type seed, It first, End last)
  noexcept -> hash_type
{
  for (auto it = first; it != last; ++it) {
    seed = hash_combine(seed, *it);
  }
  return seed;
}

//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::hash_object_representation(const T& value)
  noexcept -> hash_type
{
  return hash_combine_object_representation(hash_type{0}, value);
}

template <typename T>
inline
auto alloy::core::hash_combine_object_representation(hash_type seed, const T& value)
  noexcept -> hash_type
{
  static_assert(std::has_unique_object_representations_v<T>);

  auto p = reinterpret_cast<const std::byte*>(&value);
  auto remaining = sizeof(T);

  // Combine larger chunks of the hash at once, rather than byte-by-byte
  // This uses `hash_type` chunk-sizes to ensure that each hashed data has
  // maximum bit-spread.
  while (remaining > 0u) {
    auto chunk = hash_type{0u};
    const auto bytes = ((remaining < sizeof(hash_type)) ? remaining : sizeof(hash_type));

    std::memcpy(&chunk, p, bytes);

    seed = hash_combine(seed, chunk);

    remaining -= bytes;
    p += bytes;
  }

  return seed;
}

//------------------------------------------------------------------------------

template <typename Aggregate>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash_aggregate(const Aggregate& aggregate)
  noexcept -> hash_type
{
  return hash_combine_aggregate(hash_type{0}, aggregate);
}

template <typename Aggregate>
inline constexpr
auto alloy::core::hash_combine_aggregate(hash_type seed,
                                         const Aggregate& aggregate)
  noexcept -> hash_type
{
  static_assert(std::is_aggregate_v<Aggregate>);

  // Call 'hash_combine' on each member
  for_each_member(aggregate, [&seed](const auto& member){
    seed = hash_combine(seed, member);
  });

  return seed;
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::hash_object(const T& value)
  noexcept -> decltype(hash_value(value))
{
  // Should be redundant
  using alloy::core::hash_value;

  return hash_value(value);
}

//------------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::hash::operator()(const T& value)
  const noexcept -> decltype(hash_value(value))
{
  return hash_object(value);
}

#endif /* ALLOY_CORE_UTILITIES_HASH_HPP */
