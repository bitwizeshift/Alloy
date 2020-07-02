////////////////////////////////////////////////////////////////////////////////
/// \file quantity.hpp
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

#ifndef ALLOY_CORE_UTILITIES_QUANTITY_HPP
#define ALLOY_CORE_UTILITIES_QUANTITY_HPP

#include <type_traits> // std::make_signed_t
#include <cstddef>     // std::size_t
#include <limits>      // std::numeric_limits

namespace alloy::core {

  namespace detail {

    template <typename T, typename U>
    struct quantity_base : std::conditional<
      std::is_base_of_v<T,U>,
      T,
      U
    >{};

    template <typename T, typename U>
    using quantity_base_t = typename quantity_base<T,U>::type;

  } // namespace detail

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Represents a quantity of T objects
  ///
  /// A `quantity` object is effectively a replacement for the `size_t` type
  /// from C. It represents a quantity of a strongly typed object `T` in
  /// a discrete and countable unit.
  ///
  /// Objects of quantity<T> may operate with quantity<U> objects if T is a
  /// base class of U. This allows for the case of a quantity<Base> to count
  /// quantity<Derived> types, but not the inverse. More concretely, it allows
  /// for an example like:
  ///
  /// \code
  /// auto q = quantity<Fruit>{0};
  /// q += quantity<Apple>{5};
  /// \endcode
  ///
  /// But prevents, at compile time, code like:
  ///
  /// \code
  /// auto q = quantity<Apple>{0};
  /// q += quantity<Fruit>{5};
  /// \endcode
  ///
  /// since a quantity of a base class may abstractly want to count derived
  /// instances, but the inverse is not necessarily true (not all Fruit are
  /// Apples, but all Apples are Fruit).
  ///
  /// Comparisons of quantity objects may be done on any type, irrespective of
  /// relationship -- since it's reasonable to compare quantity<Apple> to a
  /// quantity<Orange>. Quantity objects may also be compared directly to
  /// the underlying representation, so that code in the form of:
  ///
  /// \code
  /// if (quantity<Apple>{5} == 5) { ... }
  /// \endcode
  ///
  /// Is a valid comparison.
  ///
  /// Since quantities of different objects are discretely identifiable in the
  /// type, this also allows quantities of different types to be differentiated
  /// on type-signatures for overload resolution.
  ///
  /// \note Although this type allows interop between base and derived types,
  ///       it is unable to operate on sibling types. For example:
  ///       \code
  ///       quantity<Apple>{5} + quantity<Orange>{2}
  ///       \endcode
  ///       is not able to deduce a result of quantity<Fruit>. This is due to the
  ///       C++ type system having no way of knowing a common base class between
  ///       the two, or which base should be applied.
  ///
  /// \note Unlike std::size_t, a quantity<T> object is by default _signed_.
  ///       This decision is intentional, since in modern machines an unsigned
  ///       value very seldomly is needed to represent the total number of
  ///       objects that are countable. The second template argument may be
  ///       explicitly specified to control this, if needed.
  ///
  /// \note The template argument T does not contribute at all to the type
  ///       instantiation, other than imposing limitations on what it may be
  ///       compared with. In this respect, T is a "tag" type.
  ///
  /// \tparam T the type of object being counted. The underlying type must not
  ///         be a reference, CV-qualified, or void.
  /// \tparam Rep the representation type (signed size_t by default)
  /////////////////////////////////////////////////////////////////////////////
  template <typename T, typename Rep = std::make_signed_t<std::size_t>>
  class quantity
  {
    static_assert(
      !std::is_void_v<T>,
      "quantity<void> is ill-formed."
    );

    static_assert(
      !std::is_reference_v<T>,
      "quantity<T&> is ill-formed. Use quantity<T> instead."
    );

    static_assert(
      !std::is_const_v<T>,
      "quantity<const T> is ill-formed. Quantities count discrete quantities of "
      "objects, irrespective of their constness. Use quantity<T> instead."
    );

    static_assert(
      !std::is_volatile_v<T>,
      "quantity<volatile T> is ill-formed. Quantities count discrete quantities "
      "of objects, irrespective of their constness. Use quantity<T> instead."
    );

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using counted_type = T;
    using size_type    = Rep;

    //-------------------------------------------------------------------------
    // Public Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the smallest quantity that this quantity can hold
    ///
    /// \return the smallest quantity
    static constexpr quantity min() noexcept;

    /// \brief Gets the largest quantity that this quantity can hold
    ///
    /// \return the largest quantity
    static constexpr quantity max() noexcept;

    /// \brief Gets the zero quantity
    ///
    /// \return the zero quantity
    static constexpr quantity zero() noexcept;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs this quantity
    constexpr quantity() noexcept = default;

    /// \brief Copies the quantity from \p other
    ///
    /// \param other the other quantity to copy
    constexpr quantity(const quantity& other) noexcept = default;

    /// \brief Constructs this quantity with the specified count
    ///
    /// \param count the count to construct this with
    explicit constexpr quantity(const Rep& count) noexcept;

    /// \brief Explicitly converts a quantity of U type to a quantity of T
    ///
    /// This constructor only participates in overload resolution if:
    /// * `U` is derived from `T`, and
    /// * `URep` is convertible to `Rep`
    ///
    /// \param other the other quantity being converted
    template <typename U, typename URep,
              typename = std::enable_if_t<std::is_base_of_v<T,U> && std::is_convertible_v<URep,Rep>>>
    /* IMPLICIT */ constexpr quantity(const quantity<U,URep>& other) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Copies the quantity from \p other
    ///
    /// \param other the other quantity to copy
    /// \return reference to (*this)
    constexpr quantity& operator=(const quantity& other) noexcept = default;

    /// \brief Copies and converts a quantity of U type to a quantity of T
    ///
    /// This operator only participates in overload resolution if:
    /// * `U` is derived from `T`, and
    /// * `URep` is convertible to `Rep
    ///
    /// \param other the other quantity being converted
    /// \return reference to (*this)
    template <typename U, typename URep,
              typename = std::enable_if_t<std::is_base_of_v<T,U> && std::is_convertible_v<URep,Rep>>>
    /* IMPLICIT */ constexpr quantity& operator=(const quantity<U, URep>& other) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the count from this quantitiy
    ///
    /// \return the count
    constexpr size_type count() const noexcept;

    //-------------------------------------------------------------------------
    // Unary Operators
    // ------------------------------------------------------------------------
  public:

    constexpr const quantity& operator+() const noexcept;
    constexpr quantity operator-() const noexcept;

    //-------------------------------------------------------------------------
    // Increment / Decrement Operators
    // ------------------------------------------------------------------------
  public:

    constexpr quantity& operator++() noexcept;
    constexpr quantity operator++(int) noexcept;
    constexpr quantity& operator--() noexcept;
    constexpr quantity operator--(int) noexcept;

    //-------------------------------------------------------------------------
    // Compound Arithmetic Operators
    //-------------------------------------------------------------------------
  public:

    template <typename U, typename URep, typename = std::enable_if_t<std::is_base_of_v<T,U>>>
    constexpr quantity& operator+=(const quantity<U,URep>& rhs) noexcept;

    template <typename U, typename URep, typename = std::enable_if_t<std::is_base_of_v<T,U>>>
    constexpr quantity& operator-=(const quantity<U,URep>& rhs) noexcept;

    template <typename URep>
    constexpr quantity& operator*=(const URep& rhs) noexcept;

    template <typename URep>
    constexpr quantity& operator/=(const URep& rhs) noexcept;

    template <typename U, typename URep, typename = std::enable_if_t<std::is_base_of_v<T,U>>>
    constexpr quantity& operator%=(const quantity<U,URep>& rhs) noexcept;

    template <typename URep>
    constexpr quantity& operator%=(const URep& rhs) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    size_type m_value;
  };

  //===========================================================================
  // non-member functions : class : quantity
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison Operators
  //---------------------------------------------------------------------------

  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator==(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;
  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator!=(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;
  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator<(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;
  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator>(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;
  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator<=(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;
  template <typename T, typename Rep, typename U, typename URep>
  constexpr bool operator>=(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;

  //---------------------------------------------------------------------------

  template <typename T, typename Rep>
  constexpr bool operator==(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator==(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator!=(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator!=(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator<(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator<(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator>(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator>(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator<=(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator<=(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator>=(const quantity<T,Rep>& lhs, const Rep& rhs) noexcept;
  template <typename T, typename Rep>
  constexpr bool operator>=(const Rep& lhs, const quantity<T,Rep>& rhs) noexcept;

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename T, typename Rep, typename U, typename URep,
            typename = std::enable_if_t<(std::is_base_of_v<T,U> || std::is_base_of_v<U,T>) &&
                                        (std::is_convertible_v<Rep,URep> && std::is_convertible_v<URep,Rep>)>>
  constexpr quantity<detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
    operator+(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;

  template <typename T, typename Rep, typename U, typename URep,
            typename = std::enable_if_t<(std::is_base_of_v<T,U> || std::is_base_of_v<U,T>) &&
                                        (std::is_convertible_v<Rep,URep> && std::is_convertible_v<URep,Rep>)>>
  constexpr quantity<detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
    operator-(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;

  template <typename T, typename Rep, typename URep,
            typename = std::enable_if_t<std::is_convertible_v<URep,Rep>>>
  constexpr quantity<T,Rep>
    operator*(const quantity<T,Rep>& lhs, const URep& rhs) noexcept;

  template <typename T, typename Rep, typename URep,
            typename = std::enable_if_t<std::is_convertible_v<URep,Rep>>>
  constexpr quantity<T,Rep>
    operator*(const URep& lhs, const quantity<T,Rep>& rhs) noexcept;

  template <typename T, typename Rep, typename URep,
            typename = std::enable_if_t<std::is_convertible_v<URep,Rep>>>
  constexpr quantity<T,Rep>
    operator/(const quantity<T,Rep>& lhs, const URep& rhs) noexcept;

  template <typename T, typename Rep, typename URep,
            typename = std::enable_if_t<std::is_convertible_v<URep,Rep>>>
  constexpr quantity<T,Rep>
    operator%(const quantity<T,Rep>& lhs, const URep& rhs) noexcept;

  template <typename T, typename Rep, typename U, typename URep,
            typename = std::enable_if_t<(std::is_base_of_v<T,U> || std::is_base_of_v<U,T>) &&
                                        (std::is_convertible_v<Rep,URep> && std::is_convertible_v<URep,Rep>)>>
  constexpr quantity<detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
    operator%(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs) noexcept;

} // namespace alloy::core

//=============================================================================
// definitions : class : quantity
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T,Rep> alloy::core::quantity<T, Rep>::min()
  noexcept
{
  return quantity<T,Rep>{std::numeric_limits<T>::min()};
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T,Rep> alloy::core::quantity<T, Rep>::max()
  noexcept
{
  return quantity<T,Rep>{std::numeric_limits<T>::max()};
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T,Rep> alloy::core::quantity<T, Rep>::zero()
  noexcept
{
  return quantity<T,Rep>{std::numeric_limits<T>::zero()};
}

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep>::quantity(const Rep &count)
  noexcept
  : m_value{count}
{

}

template <typename T, typename Rep>
template <typename U, typename URep, typename>
inline constexpr alloy::core::quantity<T, Rep>::quantity(const quantity<U,URep>& other)
  noexcept
  : m_value{other.count()}
{
}

//-----------------------------------------------------------------------------

template <typename T, typename Rep>
template <typename U, typename URep, typename>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator=(const quantity<U, URep>& other)
  noexcept
{
  m_value = other.count();
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr typename alloy::core::quantity<T, Rep>::size_type
  alloy::core::quantity<T, Rep>::count()
  const noexcept
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Unary Operators
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr const alloy::core::quantity<T, Rep>& alloy::core::quantity<T, Rep>::operator+()
  const noexcept
{
  return (*this);
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep> alloy::core::quantity<T, Rep>::operator-()
  const noexcept
{
  return quantity<T, Rep>{-count()};
}

//-----------------------------------------------------------------------------
// Increment / Decrement Operators
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep>& alloy::core::quantity<T, Rep>::operator++()
  noexcept
{
  ++m_value;
  return (*this);
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep> alloy::core::quantity<T, Rep>::operator++(int)
  noexcept
{
  const auto copy = (*this);
  ++m_value;
  return copy;
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep>& alloy::core::quantity<T, Rep>::operator--()
  noexcept
{
  --m_value;
  return (*this);
}

template <typename T, typename Rep>
inline constexpr alloy::core::quantity<T, Rep> alloy::core::quantity<T, Rep>::operator--(int)
  noexcept
{
  const auto copy = (*this);
  --m_value;
  return copy;
}

//-----------------------------------------------------------------------------
// Compound Operators
//-----------------------------------------------------------------------------

template <typename T, typename Rep>
template <typename U, typename URep, typename>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator+=(const alloy::core::quantity<U, URep>& rhs)
  noexcept
{
  m_value += rhs.count();
  return (*this);
}

template <typename T, typename Rep>
template <typename U, typename URep, typename>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator-=(const alloy::core::quantity<U, URep>& rhs)
  noexcept
{
  m_value -= rhs.count();
  return (*this);
}

template <typename T, typename Rep>
template <typename URep>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator*=(const URep& rhs)
  noexcept
{
  m_value *= rhs;
  return (*this);
}

template <typename T, typename Rep>
template <typename URep>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator/=(const URep& rhs)
  noexcept
{
  m_value /= rhs;
  return (*this);
}

template <typename T, typename Rep>
template <typename U, typename URep, typename>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator%=(const alloy::core::quantity<U, URep>& rhs)
  noexcept
{
  m_value %= rhs;
  return (*this);
}

template <typename T, typename Rep>
template <typename URep>
inline constexpr alloy::core::quantity<T, Rep>&
  alloy::core::quantity<T, Rep>::operator%=(const URep& rhs)
  noexcept
{
  m_value %= rhs.count();
  return (*this);
}

//=============================================================================
// definitions : non-member functions : class : quantity
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------------

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator==(const quantity<T,Rep>& lhs,
                                      const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() == rhs.count();
}

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator!=(const quantity<T,Rep>& lhs,
                                      const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() != rhs.count();
}

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator<(const quantity<T,Rep>& lhs,
                                     const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() < rhs.count();
}

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator>(const quantity<T,Rep>& lhs,
                                     const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() > rhs.count();
}

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator<=(const quantity<T,Rep>& lhs,
                                      const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() <= rhs.count();
}

template <typename T, typename Rep, typename U, typename URep>
inline constexpr bool alloy::core::operator>=(const quantity<T,Rep>& lhs,
                                      const quantity<U,URep>& rhs)
  noexcept
{
  return lhs.count() >= rhs.count();
}

//-----------------------------------------------------------------------------

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator==(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() == rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator==(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs == rhs.count();
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator!=(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() != rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator!=(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs != rhs.count();
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator<(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() < rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator<(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs < rhs.count();
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator>(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() > rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator>(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs > rhs.count();
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator<=(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() <= rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator<=(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs <= rhs.count();
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator>=(const quantity<T,Rep>& lhs, const Rep& rhs)
  noexcept
{
  return lhs.count() >= rhs;
}

template <typename T, typename Rep>
inline constexpr bool alloy::core::operator>=(const Rep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return lhs >= rhs.count();
}

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

template <typename T, typename Rep, typename U, typename URep, typename>
inline constexpr alloy::core::quantity<alloy::core::detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
  alloy::core::operator+(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs)
  noexcept
{
  using result_type = alloy::core::quantity<alloy::core::detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>;

  return result_type{lhs.count() + rhs.count()};
}

template <typename T, typename Rep, typename U, typename URep, typename>
inline constexpr alloy::core::quantity<alloy::core::detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
  alloy::core::operator-(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs)
  noexcept
{
  using result_type = alloy::core::quantity<alloy::core::detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>;

  return result_type{lhs.count() - rhs.count()};
}

template <typename T, typename Rep, typename URep, typename>
inline constexpr alloy::core::quantity<T,Rep>
  alloy::core::operator*(const quantity<T,Rep>& lhs, const URep& rhs)
  noexcept
{
  return quantity<T, Rep>{lhs.count() * rhs};
}

template <typename T, typename Rep, typename URep, typename>
inline constexpr alloy::core::quantity<T,Rep>
  alloy::core::operator*(const URep& lhs, const quantity<T,Rep>& rhs)
  noexcept
{
  return quantity<T, Rep>{lhs * rhs.count()};
}

template <typename T, typename Rep, typename URep, typename>
inline constexpr alloy::core::quantity<T,Rep>
  alloy::core::operator/(const quantity<T,Rep>& lhs, const URep& rhs)
  noexcept
{
  return quantity<T, Rep>{lhs.count() / rhs};
}

template <typename T, typename Rep, typename URep, typename>
inline constexpr alloy::core::quantity<T,Rep>
  alloy::core::operator%(const quantity<T,Rep>& lhs, const URep& rhs)
  noexcept
{
  return quantity<T, Rep>{lhs.count() % rhs};
}

template <typename T, typename Rep, typename U, typename URep, typename>
inline constexpr alloy::core::quantity<alloy::core::detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>
  alloy::core::operator%(const quantity<T,Rep>& lhs, const quantity<U,URep>& rhs)
  noexcept
{
  using result_type = quantity<detail::quantity_base_t<T,U>,std::common_type_t<Rep,URep>>;

  return result_type{lhs.count() % rhs.count()};
}

#endif /* ALLOY_CORE_UTILITIES_QUANTITY_HPP */
