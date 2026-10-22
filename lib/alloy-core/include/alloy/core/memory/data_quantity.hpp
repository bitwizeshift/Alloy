// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file data_quantity.hpp
 *
 * \brief A header for representing quantities of data (bytes, etc)
 *****************************************************************************/

#ifndef ALLOY_CORE_MEMORY_DATA_QUANTITY_HPP
#define ALLOY_CORE_MEMORY_DATA_QUANTITY_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <climits>     // CHAR_BIT
#include <concepts>    // std::convertible_to
#include <cstddef>     // std::byte
#include <cstdint>     // std::uint64_t, etc
#include <limits>      // std::numeric_limits
#include <ratio>       // std::ratio
#include <type_traits> // std::common_type

namespace alloy::core {

  //===========================================================================
  // class : data_quantity
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Represents a storage data_quantity for digital media
  ///
  /// Capacity uses the underlying 'Base' unit for handling
  ///
  /// \tparam Rep the underlying representation of this data_quantity
  /// \tparam Base The SI base unit value
  /////////////////////////////////////////////////////////////////////////////
  template <typename Rep, typename Base = std::ratio<1>>
  class data_quantity
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using rep = Rep;
    using base = typename Base::type;

    static_assert(
      base::den <= CHAR_BIT,
      "It is not possible to have a data_quantity that represents something "
      "that is smaller than 1 bit"
    );
    static_assert(
      std::is_integral<Rep>::value,
      "Representation must be an integral value; loss of precision is not "
      "supported."
    );

    //-------------------------------------------------------------------------
    // Public Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns a zero-size data_quantity
    ///
    /// \return the data_quantity
    static constexpr auto zero() noexcept -> data_quantity;

    /// \brief Returns the minimum data_quantity this can support
    ///
    /// \return the data_quantity
    static constexpr auto min() noexcept -> data_quantity;

    /// \brief Returns the maximum data_quantity this can support
    ///
    /// \return the data_quantity
    static constexpr auto max() noexcept -> data_quantity;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this data_quantity
    constexpr data_quantity() noexcept = default;

    /// \brief Constructs this data_quantity from an underlying representation
    ///
    /// \param r the value
    template <typename URep>
      requires std::convertible_to<URep, Rep>
    constexpr explicit data_quantity(const URep& r) noexcept;

    /// \brief Converts \p other to this data_quantity
    ///
    /// This constructor is only enabled if there will be no
    /// loss-of-precision. To have a lossy conversion, see data_quantity_cast
    ///
    /// \param other the other data_quantity to convert
    template <typename URep, typename UBase>
      requires (std::ratio_divide<UBase, Base>::den == 1 &&
                std::convertible_to<URep, Rep>)
    constexpr data_quantity(data_quantity<URep,UBase> other) noexcept;

    /// \brief Copy-constructs this \p data_quantity from \p other
    ///
    /// \param other the other data_quantity to copy
    constexpr data_quantity(const data_quantity& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this
    ///
    /// \param other the other data_quantity to copy
    /// \return reference to `(*this)`
    auto operator=(const data_quantity& other)
      noexcept -> data_quantity& = default;

    //-------------------------------------------------------------------------
    // Arithmetic Operators
    //-------------------------------------------------------------------------
  public:
    constexpr auto operator+()
      const noexcept -> std::common_type_t<data_quantity>;
    constexpr auto operator-()
      const noexcept -> std::common_type_t<data_quantity>;

    constexpr auto operator++() noexcept -> data_quantity&;
    constexpr auto operator++(int) noexcept -> data_quantity;
    constexpr auto operator--() noexcept -> data_quantity&;
    constexpr auto operator--(int) noexcept -> data_quantity;

    constexpr auto operator+=(const data_quantity& rhs)
      noexcept -> data_quantity&;
    constexpr auto operator-=(const data_quantity& rhs)
      noexcept -> data_quantity&;
    constexpr auto operator*=(const rep& r) noexcept -> data_quantity&;
    constexpr auto operator/=(const rep& r) noexcept -> data_quantity&;
    constexpr auto operator%=(const rep& r) noexcept -> data_quantity&;
    constexpr auto operator%=(const data_quantity& rhs)
      noexcept -> data_quantity&;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the total count of the data_quantity
    ///
    /// \return the count
    constexpr auto count() const noexcept -> Rep;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

      Rep m_count;
  };


  //===========================================================================
  // aliases : class : std::ratio
  //===========================================================================

  // SI units

  inline namespace si {

    using kilo = std::kilo;
    using mega = std::mega;
    using giga = std::giga;
    using tera = std::tera;
    using peta = std::peta;
    using exa  = std::exa;

    using kibi = std::ratio<1024>;
    using mebi = std::ratio_multiply<std::ratio<1024>,kibi>;
    using gibi = std::ratio_multiply<std::ratio<1024>,mebi>;
    using tebi = std::ratio_multiply<std::ratio<1024>,gibi>;
    using pebi = std::ratio_multiply<std::ratio<1024>,tebi>;
    using exbi = std::ratio_multiply<std::ratio<1024>,pebi>;

  } // namespace si

  //===========================================================================
  // trait : is_data_quantity
  //===========================================================================

  template <typename T>
  struct is_data_quantity : std::false_type{};

  template <typename Rep, typename Base>
  struct is_data_quantity<data_quantity<Rep,Base>> : std::true_type{};

  template <typename T>
  inline constexpr bool is_data_quantity_v = is_data_quantity<T>::value;

  //===========================================================================
  // aliases : class : data_quantity
  //===========================================================================

  //---------------------------------------------------------------------------
  // Byte Units
  //---------------------------------------------------------------------------

  // should 'bytes' be 'std::size_t'?
  using bytes     = data_quantity<std::uint64_t>;           // at least 64 bits
  using kilobytes = data_quantity<std::uint64_t, si::kilo>; // at least 54 bits
  using megabytes = data_quantity<std::uint64_t, si::mega>; // at least 44 bits
  using gigabytes = data_quantity<std::uint64_t, si::giga>; // at least 34 bits
  using terabytes = data_quantity<std::uint32_t, si::tera>; // at least 24 bits
  using petabytes = data_quantity<std::uint16_t, si::peta>; // at least 16 bits

  using kibibytes = data_quantity<std::uint64_t, si::kibi>; // at least 54 bits
  using mebibytes = data_quantity<std::uint64_t, si::mebi>; // at least 44 bits
  using gibibytes = data_quantity<std::uint64_t, si::gibi>; // at least 34 bits
  using tebibytes = data_quantity<std::uint32_t, si::tebi>; // at least 24 bits
  using pebibytes = data_quantity<std::uint16_t, si::pebi>; // at least 16 bits

  //---------------------------------------------------------------------------
  // Bit Units
  //---------------------------------------------------------------------------

  /// \brief The smallest / atomic unit
  using bits      = data_quantity<std::uint64_t, std::ratio<1, CHAR_BIT>>;
  using kilobits  = data_quantity<std::uint64_t, std::ratio<si::kilo::num, CHAR_BIT>::type>;
  using megabits  = data_quantity<std::uint64_t, std::ratio<si::mega::num, CHAR_BIT>::type>;
  using gigabits  = data_quantity<std::uint64_t, std::ratio<si::giga::num, CHAR_BIT>::type>;
  using terabits  = data_quantity<std::uint32_t, std::ratio<si::tera::num, CHAR_BIT>::type>;
  using petabits  = data_quantity<std::uint32_t, std::ratio<si::peta::num, CHAR_BIT>::type>;

  //---------------------------------------------------------------------------

  using kibibits  = data_quantity<std::uint64_t, std::ratio<si::kibi::num, CHAR_BIT>::type>;
  using mebibits  = data_quantity<std::uint64_t, std::ratio<si::mebi::num, CHAR_BIT>::type>;
  using gibibits  = data_quantity<std::uint64_t, std::ratio<si::gibi::num, CHAR_BIT>::type>;
  using tebibits  = data_quantity<std::uint32_t, std::ratio<si::tebi::num, CHAR_BIT>::type>;
  using pebibits  = data_quantity<std::uint32_t, std::ratio<si::pebi::num, CHAR_BIT>::type>;

  //---------------------------------------------------------------------------
  // Non-standard units
  //---------------------------------------------------------------------------

  // A nibble is defined as half of a byte (irrespective of the number of bits
  // in a byte)
  using nibbles = data_quantity<std::uint64_t, std::ratio<1,2>>;

  //===========================================================================
  // non-member functions : class : data_quantity
  //===========================================================================

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator+(
    const data_quantity<Rep1,Base1>& lhs,
    const data_quantity<Rep2,Base2>& rhs
  ) noexcept
    -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator-(
    const data_quantity<Rep1,Base1>& lhs,
    const data_quantity<Rep2,Base2>& rhs
  ) noexcept
    -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>;

  template <typename Rep1, typename Base, typename Rep2>
  constexpr auto operator*(const data_quantity<Rep1,Base>& d, const Rep2& s)
    noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  template <typename Rep1, typename Rep2, typename Base>
  constexpr auto operator*(const Rep1& s, const data_quantity<Rep2,Base>& d)
    noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  template <typename Rep1, typename Base, typename Rep2>
  constexpr auto operator/(const data_quantity<Rep1,Base>& d, const Rep2& s)
    noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator/(
    const data_quantity<Rep1,Base1>& lhs,
    const data_quantity<Rep2,Base2>& rhs
  ) noexcept -> std::common_type_t<Rep1,Rep2>;

  template <typename Rep1, typename Base, typename Rep2>
  constexpr auto operator%(const data_quantity<Rep1, Base>& d, const Rep2& s)
    noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator%(
    const data_quantity<Rep1,Base1>& lhs,
    const data_quantity<Rep2,Base2>& rhs
  ) noexcept
    -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>;

  //---------------------------------------------------------------------------
  // Comparison Operators
  //---------------------------------------------------------------------------

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator==(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator!=(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator<(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator<=(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator>(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto operator>=(
    const data_quantity<Rep1, Base1>& lhs,
    const data_quantity<Rep2, Base2>& rhs
  ) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Converts \p from to a ToCapacity with lossy conversion
  ///
  /// This is the only way to convert types with loss of precision.
  ///
  /// \tparam ToCapacity the data_quantity to convert to
  /// \param from the data_quantity to convert
  /// \return the converted data_quantity
  template <typename ToCapacity, typename FromCapacity>
    requires (alloy::core::is_data_quantity_v<ToCapacity> &&
              alloy::core::is_data_quantity_v<FromCapacity>)
  constexpr auto data_quantity_cast(const FromCapacity& from) -> ToCapacity;

  //---------------------------------------------------------------------------

  /// \brief Computes the absolute difference between \p lhs and \p rhs
  ///
  /// \param lhs the left data_quantity
  /// \param rhs the right data_quantity
  /// \return the difference between the two capacities
  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  constexpr auto difference(
    const data_quantity<Rep1,Base1>& lhs,
    const data_quantity<Rep2,Base2>& rhs
  ) noexcept
    -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>;

  /// \brief Rounds the specified data_quantity down to ToCapacity
  ///
  /// \tparam ToCapacity the data_quantity to round down to
  /// \param c the data_quantity
  /// \return the rounded data_quantity
  template <typename ToCapacity, typename Rep, typename Base>
    requires alloy::core::is_data_quantity_v<ToCapacity>
  constexpr auto floor(const data_quantity<Rep, Base>& c)
    noexcept -> ToCapacity;


  /// \brief Rounds the specified data_quantity up to ToCapacity
  ///
  /// \tparam ToCapacity the data_quantity to round up to
  /// \param c the data_quantity
  /// \return the rounded data_quantity
  template <typename ToCapacity, typename Rep, typename Base>
    requires alloy::core::is_data_quantity_v<ToCapacity>
  constexpr auto ceil(const data_quantity<Rep, Base>& c) noexcept -> ToCapacity;

  /// \brief Rounds the specified data_quantity \p c to the nearest ToCapacity
  ///
  /// This follows the typical C++ rounding model, and behaves like std::round
  /// by always rounding away from 0.
  ///
  /// \tparam ToCapacity the data_quantity to round to
  /// \param c the data_quantity to round
  /// \return the rounded data_quantity
  template <typename ToCapacity, typename Rep, typename Base>
    requires alloy::core::is_data_quantity_v<ToCapacity>
  constexpr auto round(const data_quantity<Rep, Base>& c)
    noexcept -> ToCapacity;

  /// \brief Gets the absolute value of the specified data_quantity
  ///
  /// \param c the data_quantity to get the absolute value of
  /// \return the absolute value of the data_quantity
  template <typename Rep, typename Base>
  constexpr auto abs(const data_quantity<Rep, Base>& c)
    noexcept -> data_quantity<Rep, Base>;

  //---------------------------------------------------------------------------

  /// \brief Gets the size of the specified type, in bytes
  ///
  /// \tparam T the type to get the size of
  template <typename T>
  constexpr auto size_of() noexcept -> bytes;

  /// \brief Gets the size of the specified value, in bytes
  ///
  /// \param v the value to get the size of
  template <typename T>
  constexpr auto size_of(const T& v) noexcept -> bytes;

  //===========================================================================
  // literals : class : data_quantity
  //===========================================================================

  inline namespace literals {
    //-------------------------------------------------------------------------
    // Byte Literals
    //-------------------------------------------------------------------------

    inline namespace byte_literals {

      constexpr auto operator""_b(unsigned long long x) noexcept -> bytes;
      constexpr auto operator""_kb(unsigned long long x) noexcept -> kilobytes;
      constexpr auto operator""_mb(unsigned long long x) noexcept -> megabytes;
      constexpr auto operator""_gb(unsigned long long x) noexcept -> gigabytes;
      constexpr auto operator""_tb(unsigned long long x) noexcept -> terabytes;
      constexpr auto operator""_pb(unsigned long long x) noexcept -> petabytes;

      //-----------------------------------------------------------------------

      constexpr auto operator""_kib(unsigned long long x) noexcept -> kibibytes;
      constexpr auto operator""_mib(unsigned long long x) noexcept -> mebibytes;
      constexpr auto operator""_gib(unsigned long long x) noexcept -> gibibytes;
      constexpr auto operator""_tib(unsigned long long x) noexcept -> tebibytes;
      constexpr auto operator""_pib(unsigned long long x) noexcept -> pebibytes;

    } // namespace byte_literals

    //-------------------------------------------------------------------------
    // Bit Literals
    //-------------------------------------------------------------------------

    inline namespace bit_literals {

      constexpr auto operator""_bit(unsigned long long x) noexcept -> bits;
      constexpr auto operator""_kbit(unsigned long long x) noexcept -> kilobits;
      constexpr auto operator""_mbit(unsigned long long x) noexcept -> megabits;
      constexpr auto operator""_gbit(unsigned long long x) noexcept -> gigabits;
      constexpr auto operator""_tbit(unsigned long long x) noexcept -> terabits;
      constexpr auto operator""_pbit(unsigned long long x) noexcept -> petabits;

      //-----------------------------------------------------------------------

      constexpr auto operator""_kibit(unsigned long long x)
        noexcept -> kibibits;
      constexpr auto operator""_mibit(unsigned long long x)
        noexcept -> mebibits;
      constexpr auto operator""_gibit(unsigned long long x)
        noexcept -> gibibits;
      constexpr auto operator""_tibit(unsigned long long x)
        noexcept -> tebibits;
      constexpr auto operator""_pibit(unsigned long long x)
        noexcept -> pebibits;

    } // inline namespace data_quantity_literals
  } // inline namespace literals

  //===========================================================================
  // non-member functions : alias : data_quantity
  //===========================================================================

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename Rep, typename Base>
    requires (Base::den == 1)
  constexpr auto operator+(std::byte* p, data_quantity<Rep,Base> offset)
    noexcept -> std::byte*;

  template <typename Rep, typename Base>
    requires (Base::den == 1)
  constexpr auto operator+(const std::byte* p, data_quantity<Rep,Base> offset)
    noexcept -> const std::byte*;

  template <typename Rep, typename Base>
    requires (Base::den == 1)
  constexpr auto operator-(std::byte* p, data_quantity<Rep,Base> offset)
    noexcept -> std::byte*;

  template <typename Rep, typename Base>
    requires (Base::den == 1)
  constexpr auto operator-(const std::byte* p, data_quantity<Rep,Base> offset)
    noexcept -> const std::byte*;

} // namespace alloy::core

namespace alloy::core::detail {

  template<std::intmax_t X, std::intmax_t Y>
  struct gcd : gcd<Y, X % Y>{};

  template<std::intmax_t X>
  struct gcd<X,0> : std::integral_constant<std::intmax_t, X>{};

  template<std::intmax_t X, std::intmax_t Y>
	struct lcm : std::integral_constant<std::intmax_t, (X / gcd<X,Y>::value) * Y>{};
} // namespace alloy::core::detail

namespace std {

  // Compute the common type of two capacities
  template <typename Rep1, typename Base1, typename Rep2, typename Base2>
  struct common_type<alloy::core::data_quantity<Rep1, Base1>, alloy::core::data_quantity<Rep2, Base2>>
  {
    using type = alloy::core::data_quantity<
      std::common_type_t<Rep1, Rep2>,
      std::ratio<
        alloy::core::detail::gcd<Base1::num, Base2::num>::value,
        alloy::core::detail::lcm<Base1::den, Base2::den>::value
      >
    >;
  };

} // namespace std

//-----------------------------------------------------------------------------
// Public Static Factories
//-----------------------------------------------------------------------------

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::zero()
  noexcept -> data_quantity<Rep,Base>
{
  return data_quantity{0};
}

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::min()
  noexcept -> data_quantity<Rep,Base>
{
  return data_quantity{std::numeric_limits<Rep>::min()};
}

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::max()
  noexcept -> data_quantity<Rep,Base>
{
  return data_quantity{std::numeric_limits<Rep>::max()};
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename Rep, typename Base>
template <typename URep>
  requires std::convertible_to<URep, Rep>
inline constexpr
alloy::core::data_quantity<Rep,Base>::data_quantity(const URep& r) noexcept
  : m_count{static_cast<Rep>(r)}
{

}

template <typename Rep, typename Base>
template <typename URep, typename UBase>
  requires (std::ratio_divide<UBase, Base>::den == 1 &&
           std::convertible_to<URep, Rep>)
inline constexpr
alloy::core::data_quantity<Rep,Base>::data_quantity(
  data_quantity<URep,UBase> other
) noexcept
  : m_count{static_cast<Rep>(other.count() * std::ratio_divide<UBase, Base>::num)}
{

}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator+()
  const noexcept -> std::common_type_t<data_quantity<Rep,Base>>
{
  using type = std::common_type_t<data_quantity>;

  return type{m_count};
}

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator-()
  const noexcept -> std::common_type_t<data_quantity<Rep,Base>>
{
  using type = std::common_type_t<data_quantity>;

  return type{-m_count};
}

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator++()
  noexcept -> data_quantity<Rep,Base>&
{
  ++m_count;
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator++(int)
  noexcept -> data_quantity<Rep,Base>
{
  const auto copy = (*this);
  ++m_count;
  return copy;
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator--()
  noexcept -> data_quantity<Rep,Base>&
{
  --m_count;
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator--(int)
  noexcept -> data_quantity<Rep,Base>
{
  const auto copy = (*this);
  --m_count;
  return copy;
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator+=(const data_quantity& rhs)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count += rhs.count();
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator-=(const data_quantity& rhs)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count -= rhs.count();
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator*=(const rep& r)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count *= r;
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator/=(const rep& r)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count /= r;
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator%=(const rep& r)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count %= r;
  return (*this);
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::operator%=(const data_quantity& rhs)
  noexcept -> data_quantity<Rep,Base>&
{
  m_count %= rhs.count();
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename Rep, typename Base>
inline constexpr
auto alloy::core::data_quantity<Rep,Base>::count() const noexcept -> Rep
{
  return m_count;
}


//=============================================================================
// definitions : non-member functions : class : data_quantity
//=============================================================================

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator+(
  const data_quantity<Rep1,Base1>& lhs,
  const data_quantity<Rep2,Base2>& rhs
) noexcept
  -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>
{
  using result_type = std::common_type_t<
    data_quantity<Rep1,Base1>,
    data_quantity<Rep2,Base2>
  >;

  return result_type{
    result_type{lhs}.count() + result_type{rhs}.count()
  };
}

template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator-(
  const data_quantity<Rep1,Base1>& lhs,
  const data_quantity<Rep2,Base2>& rhs
) noexcept
  -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>
{
  using result_type = std::common_type_t<
    data_quantity<Rep1,Base1>,
    data_quantity<Rep2,Base2>
  >;

  return result_type{
    result_type{lhs}.count() - result_type{rhs}.count()
  };
}


template <typename Rep1, typename Base, typename Rep2>
inline constexpr
auto alloy::core::operator*(
  const data_quantity<Rep1,Base>& lhs,
  const Rep2& rhs
) noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>
{
  using result_type = data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  return result_type{
    result_type{lhs}.count() * rhs
  };
}


template <typename Rep1, typename Rep2, typename Base>
inline constexpr
auto alloy::core::operator*(
  const Rep1& lhs,
  const data_quantity<Rep2,Base>& rhs
) noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>
{
  using result_type = data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  return result_type{
    lhs * result_type{rhs}.count()
  };
}


template <typename Rep1, typename Base, typename Rep2>
inline constexpr
auto alloy::core::operator/(
  const data_quantity<Rep1,Base>& lhs,
  const Rep2& rhs
) noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>
{
  using result_type = data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  return result_type{
    result_type{lhs}.count() / rhs
  };
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator/(
  const data_quantity<Rep1,Base1>& lhs,
  const data_quantity<Rep2,Base2>& rhs
) noexcept -> std::common_type_t<Rep1,Rep2>
{
  using result_type = std::common_type_t<Rep1,Rep2>;
  using common_type = std::common_type_t<
    data_quantity<Rep1,Base1>,
    data_quantity<Rep2,Base2>
  >;

  return result_type{
    common_type{lhs}.count() / common_type{rhs}.count()
  };
}


template <typename Rep1, typename Base, typename Rep2>
inline constexpr
auto alloy::core::operator%(
  const data_quantity<Rep1, Base>& lhs,
  const Rep2& rhs
) noexcept -> data_quantity<std::common_type_t<Rep1,Rep2>, Base>
{
  using result_type = data_quantity<std::common_type_t<Rep1,Rep2>, Base>;

  return result_type{
    result_type{lhs}.count() % rhs
  };
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator%(
  const data_quantity<Rep1,Base1>& lhs,
  const data_quantity<Rep2,Base2>& rhs
) noexcept
  -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>
{
  using result_type = std::common_type_t<
    data_quantity<Rep1,Base1>,
    data_quantity<Rep2,Base2>
  >;

  return result_type{
    result_type{lhs}.count() % result_type{rhs}.count()
  };
}


//-----------------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------------

template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator==(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() == common_type{rhs}.count();
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator!=(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() != common_type{rhs}.count();
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator<(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() < common_type{rhs}.count();
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator<=(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() <= common_type{rhs}.count();
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator>(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() > common_type{rhs}.count();
}


template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::operator>=(
  const data_quantity<Rep1, Base1>& lhs,
  const data_quantity<Rep2, Base2>& rhs
) noexcept -> bool
{
  using common_type = std::common_type_t<
    data_quantity<Rep1, Base1>,
    data_quantity<Rep2, Base2>
  >;

  return common_type{lhs}.count() >= common_type{rhs}.count();
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename ToCapacity, typename FromCapacity>
  requires (alloy::core::is_data_quantity_v<ToCapacity> &&
           alloy::core::is_data_quantity_v<FromCapacity>)
inline constexpr
auto alloy::core::data_quantity_cast(const FromCapacity& from) -> ToCapacity
{
  using from_base = typename FromCapacity::base;
  using to_base = typename ToCapacity::base;
  using to_rep = typename ToCapacity::rep;
  using result = std::ratio_divide<from_base, to_base>;

  const auto num = static_cast<to_rep>(from.count() * result::num);
  const auto den = static_cast<to_rep>(result::den);

  return ToCapacity{num / den};
}

//---------------------------------------------------------------------------

template <typename Rep1, typename Base1, typename Rep2, typename Base2>
inline constexpr
auto alloy::core::difference(
  const data_quantity<Rep1,Base1>& lhs,
  const data_quantity<Rep2,Base2>& rhs
) noexcept
  -> std::common_type_t<data_quantity<Rep1,Base1>, data_quantity<Rep2,Base2>>
{
  using result_type = std::common_type_t<
    data_quantity<Rep1,Base1>,
    data_quantity<Rep2,Base2>
  >;

  const auto l = result_type{lhs};
  const auto r = result_type{rhs};

  if (l < r) {
    return r - l;
  }
  return l - r;
}


template <typename ToCapacity, typename Rep, typename Base>
  requires alloy::core::is_data_quantity_v<ToCapacity>
inline constexpr
auto alloy::core::floor(const data_quantity<Rep, Base>& c)
  noexcept -> ToCapacity
{
  const auto to = data_quantity_cast<ToCapacity>(c);
  if (to > c) {
    return to - ToCapacity{1};
  }
  return to;
}


template <typename ToCapacity, typename Rep, typename Base>
  requires alloy::core::is_data_quantity_v<ToCapacity>
inline constexpr
auto alloy::core::ceil(const data_quantity<Rep, Base>& c) noexcept -> ToCapacity
{
  const auto to = data_quantity_cast<ToCapacity>(c);
  if (to < c) {
    return to + ToCapacity{1};
  }
  return to;
}


template <typename ToCapacity, typename Rep, typename Base>
  requires alloy::core::is_data_quantity_v<ToCapacity>
inline constexpr
auto alloy::core::round(const data_quantity<Rep, Base>& c)
  noexcept -> ToCapacity
{
  const auto lower = floor<ToCapacity>(c);
  const auto upper = lower + ToCapacity{1};

  const auto lower_diff = difference(c, lower);
  const auto upper_diff = difference(upper, c);

  // Differences are the same -- means either 'c' is already rounded,
  // or that it's dead-center -- which should be rounded up to behave like
  // the standard rounding that C++ offers (0.5 -> 1, -0.5 -> -1)
  if (lower_diff == upper_diff) {
    if (upper > ToCapacity::zero()) {
      return upper;
    }
    return lower;
  }

  // round to the closer difference
  if (lower_diff < upper_diff) {
    return lower;
  }
  return upper;
}


template <typename Rep, typename Base>
inline constexpr
auto alloy::core::abs(const data_quantity<Rep, Base>& c)
  noexcept -> data_quantity<Rep, Base>
{
  return c >= data_quantity<Rep, Base>::zero() ? c : -c;
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::size_of() noexcept -> bytes
{
  return bytes{sizeof(T)};
}


template <typename T>
inline constexpr
auto alloy::core::size_of(const T& v) noexcept -> bytes
{
  return bytes{sizeof(v)};
}

//=============================================================================
// literals : class : data_quantity
//=============================================================================

//-----------------------------------------------------------------------------
// Byte Literals
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::literals::byte_literals::operator""_b(unsigned long long x)
  noexcept -> bytes
{
  return bytes{static_cast<bytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_kb(unsigned long long x)
  noexcept -> kilobytes
{
  return kilobytes{static_cast<kilobytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_mb(unsigned long long x)
  noexcept -> megabytes
{
  return megabytes{static_cast<megabytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_gb(unsigned long long x)
  noexcept -> gigabytes
{
  return gigabytes{static_cast<gigabytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_tb(unsigned long long x)
  noexcept -> terabytes
{
  return terabytes{static_cast<terabytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_pb(unsigned long long x)
  noexcept -> petabytes
{
  return petabytes{static_cast<petabytes::rep>(x)};
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::literals::byte_literals::operator""_kib(unsigned long long x)
  noexcept -> kibibytes
{
  return kibibytes{static_cast<kibibytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_mib(unsigned long long x)
  noexcept -> mebibytes
{
  return mebibytes{static_cast<mebibytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_gib(unsigned long long x)
  noexcept -> gibibytes
{
  return gibibytes{static_cast<gibibytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_tib(unsigned long long x)
  noexcept -> tebibytes
{
  return tebibytes{static_cast<tebibytes::rep>(x)};
}


inline constexpr
auto alloy::core::literals::byte_literals::operator""_pib(unsigned long long x)
  noexcept -> pebibytes
{
  return pebibytes{static_cast<pebibytes::rep>(x)};
}

//-----------------------------------------------------------------------------
// Bit Literals
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::literals::bit_literals::operator""_bit(unsigned long long x)
  noexcept -> bits
{
  return bits{static_cast<bits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_kbit(unsigned long long x)
  noexcept -> kilobits
{
  return kilobits{static_cast<kilobits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_mbit(unsigned long long x)
  noexcept -> megabits
{
  return megabits{static_cast<megabits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_gbit(unsigned long long x)
  noexcept -> gigabits
{
  return gigabits{static_cast<gigabits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_tbit(unsigned long long x)
  noexcept -> terabits
{
  return terabits{static_cast<terabits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_pbit(unsigned long long x)
  noexcept -> petabits
{
  return petabits{static_cast<petabits::rep>(x)};
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::literals::bit_literals::operator""_kibit(unsigned long long x)
  noexcept -> kibibits
{
  return kibibits{static_cast<kibibits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_mibit(unsigned long long x)
  noexcept -> mebibits
{
  return mebibits{static_cast<mebibits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_gibit(unsigned long long x)
  noexcept -> gibibits
{
  return gibibits{static_cast<gibibits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_tibit(unsigned long long x)
  noexcept -> tebibits
{
  return tebibits{static_cast<tebibits::rep>(x)};
}


inline constexpr
auto alloy::core::literals::bit_literals::operator""_pibit(unsigned long long x)
  noexcept -> pebibits
{
  return pebibits{static_cast<pebibits::rep>(x)};
}

template <typename Rep, typename Base>
  requires (Base::den == 1)
inline constexpr
auto alloy::core::operator+(std::byte* p, data_quantity<Rep,Base> offset)
  noexcept -> std::byte*
{
  const auto b = bytes{offset};
  return p + b.count();
}

template <typename Rep, typename Base>
  requires (Base::den == 1)
inline constexpr
auto alloy::core::operator+(const std::byte* p, data_quantity<Rep,Base> offset)
  noexcept -> const std::byte*
{
  const auto b = bytes{offset};
  return p + b.count();
}


template <typename Rep, typename Base>
  requires (Base::den == 1)
inline constexpr
auto alloy::core::operator-(std::byte* p, data_quantity<Rep,Base> offset)
  noexcept -> std::byte*
{
  const auto b = bytes{offset};
  return p - b.count();
}

template <typename Rep, typename Base>
  requires (Base::den == 1)
inline constexpr
auto alloy::core::operator-(const std::byte* p, data_quantity<Rep,Base> offset)
  noexcept -> const std::byte*
{
  const auto b = bytes{offset};
  return p - b.count();
}

#endif /* ALLOY_CORE_MEMORY_DATA_QUANTITY_HPP */

