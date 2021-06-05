////////////////////////////////////////////////////////////////////////////////
/// \file optional.hpp
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

#ifndef ALLOY_CORE_UTILITIES_OPTIONAL_HPP
#define ALLOY_CORE_UTILITIES_OPTIONAL_HPP

#include "alloy/core/utilities/result.hpp"

#include <optional> // std::optional
#include <utility>  // std::reference_wrapper

namespace alloy::core {

  using null_type = std::nullopt_t;
  inline constexpr auto& null = std::nullopt;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type that represents a nullable object.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class optional : public std::optional<T>
  {
    using base_type = std::optional<T>;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    using base_type::base_type;

    /// \brief Allow construction from an optional reference
    ///
    /// \param other the other object to attempt to use to construct this
    template <typename U, typename=std::enable_if_t<std::is_constructible_v<T,U&>>>
    explicit optional(const optional<U&> other);
    optional(const optional&) = default;
    optional(optional&&) = default;

    //--------------------------------------------------------------------------

    using base_type::operator=;

    auto operator=(const optional&) -> optional& = default;
    auto operator=(optional&&) -> optional& = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    using base_type::emplace;
    using base_type::reset;
    using base_type::swap;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    using base_type::operator*;
    using base_type::operator->;

    using base_type::value;

    using base_type::has_value;
    using base_type::operator bool;

    //--------------------------------------------------------------------------
    // Monadic
    //--------------------------------------------------------------------------
  public:

    using base_type::value_or;

    /// \{
    /// \brief Maps the reference contained in this optional to an optional
    ///        of the result of `fn(**this)`
    ///
    /// \param fn the function to use for mapping
    /// \return the mapped optional
    template <typename Fn>
    constexpr auto map(Fn&& fn) const & -> optional<std::invoke_result_t<Fn,const T&>>;
    template <typename Fn>
    constexpr auto map(Fn&& fn) && -> optional<std::invoke_result_t<Fn,T&&>>;
    /// \}

    /// \{
    /// \brief Maps the reference contained in this optional to the result of
    ///        `fn(**this)`
    ///
    /// \param fn the function to use for mapping
    /// \return the result of `fn(**this)` or `null`
    template <typename Fn>
    constexpr auto flat_map(Fn&& fn) const & -> std::invoke_result_t<Fn,const T&>;
    template <typename Fn>
    constexpr auto flat_map(Fn&& fn) && -> std::invoke_result_t<Fn,T&&>;
    /// \}

    /// \{
    /// \brief If `has_value()` is `true`, returns `optional<U>` containing
    ///        \p u
    ///
    /// \param u the value to construct the optional from, if it contains a value
    /// \return the mapped optional
    template <typename U>
    constexpr auto and_then(U&& u) const -> optional<std::decay_t<U>>;
    template <typename U>
    constexpr auto and_then(std::reference_wrapper<U> ref) const -> optional<U&>;
    /// \}
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An optional reference
  ///
  /// This behaves similarly to `optional<T>`, except with limited support for
  /// construction, and it only works via indirection.
  ///
  /// An `optional<T&>` is effecitvely a rebindable reference -- much like a raw
  /// pointer is. Unlike a pointer, this still continues to provide the
  /// various monadic access functionalities along with a queriable interface.
  /// Additionally, optional references are interoperable with optional values
  /// -- which allows for an optional value to be constructed from an optional
  /// reference argument for conditionally indirect construction.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class optional<T&>
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type = T&;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this optional reference without a binding
    constexpr optional() noexcept;

    /// \brief Constructs this optional reference by binding \p ref to it
    ///
    /// \param ref the reference to bind
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U&,T&>>>
    constexpr optional(U& ref) noexcept;

    /// \brief Constructs this optional reference without a binding
    constexpr optional(null_type) noexcept;

    optional(const optional& other) = default;
    optional(optional&& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Rebinds this optional reference to refer to \p ref
    ///
    /// \param ref the new reference to bind
    /// \return reference to (*this)
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U&,T&>>>
    auto operator=(U& ref) noexcept -> optional&;

    /// \brief Rebinds this optional reference to null
    ///
    /// \return reference to (*this)
    auto operator=(null_type) noexcept -> optional&;

    auto operator=(const optional& other) -> optional& = default;
    auto operator=(optional&& other) -> optional& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Accesses the bound reference through indirect operations
    constexpr auto operator->() const noexcept -> T*;
    constexpr auto operator*() const noexcept -> T&;
    /// \}

    /// \{
    /// \brief Queries whether this optional contains a value (is bound)
    ///
    /// \return true if this contains a value
    constexpr auto has_value() const noexcept -> bool;
    constexpr operator bool() const noexcept;
    /// \}

    /// \brief Gets the underlying reference, if one is bound
    ///
    /// \throw bad_optional_access on failure
    /// \return the underlying bound reference
    constexpr auto value() const -> T&;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------

    /// \brief Sets the current state to null
    constexpr auto reset() noexcept -> void;

    //--------------------------------------------------------------------------
    // Monadic
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the currently bound reference if one exists, or \p ref if
    ///        one does not
    ///
    /// \param ref the other reference to use on empty
    /// \return one of the two references
    template <typename U>
    constexpr auto value_or(U& ref) const noexcept -> T&;

    /// \brief Maps the reference contained in this optional to an optional
    ///        of the result of `fn(**this)`
    ///
    /// \param fn the function to use for mapping
    /// \return the mapped optional
    template <typename Fn>
    auto map(Fn&& fn) const -> optional<std::invoke_result_t<Fn,T&>>;

    /// \brief Maps the reference contained in this optional to the result of
    ///        `fn(**this)`
    ///
    /// \param fn the function to use for mapping
    /// \return the result of `fn(**this)` or `null`
    template <typename Fn>
    auto flat_map(Fn&& fn) const -> std::invoke_result_t<Fn,T&>;

    /// \{
    /// \brief If `has_value()` is `true`, returns `optional<U>` containing
    ///        \p u
    ///
    /// \param u the value to construct the optional from, if it contains a value
    /// \return the mapped optional
    template <typename U>
    constexpr auto and_then(U&& u) const -> optional<std::decay_t<U>>;
    template <typename U>
    constexpr auto and_then(std::reference_wrapper<U> ref) const -> optional<U&>;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    T* m_data;
  };

  //============================================================================
  // non-member functions : class : optional
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison
  //----------------------------------------------------------------------------

  template <typename T, typename U>
  constexpr auto operator==(const optional<T>& lhs,
                            const optional<U>& rhs) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator!=(const optional<T>& lhs,
                            const optional<U>& rhs) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<(const optional<T>& lhs,
                           const optional<U>& rhs) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>(const optional<T>& lhs,
                           const optional<U>& rhs) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<=(const optional<T>& lhs,
                            const optional<U>& rhs) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>=(const optional<T>& lhs,
                            const optional<U>& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  template <typename T, typename U>
  constexpr auto operator==(const optional<T>& opt,
                            const U& value) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator!=(const optional<T>& opt,
                            const U& value) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<(const optional<T>& opt,
                           const U& value) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>(const optional<T>& opt,
                           const U& value) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<=(const optional<T>& opt,
                            const U& value) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>=(const optional<T>& opt,
                            const U& value) noexcept -> bool;

  //----------------------------------------------------------------------------

  template <typename T, typename U>
  constexpr auto operator==(const T& value,
                            const optional<U>& opt) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator!=(const T& lhs,
                            const optional<U>& opt) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<(const T& lhs,
                           const optional<U>& opt) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>(const T& lhs,
                           const optional<U>& opt) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator<=(const T& lhs,
                            const optional<U>& opt) noexcept -> bool;
  template <typename T, typename U>
  constexpr auto operator>=(const T& lhs,
                            const optional<U>& opt) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \{
  /// \brief Makes an optional object
  template <typename T>
  constexpr auto make_optional(T&& v) -> optional<std::decay_t<T>>;
  template <typename T, typename...Args>
  constexpr auto make_optional(Args&&...args) -> optional<T>;
  template <typename T>
  constexpr auto make_optional(std::reference_wrapper<T> ref) -> optional<T&>;
  /// \}

  /// \{
  /// \brief Converts a result into an optional type
  ///
  /// \param r the result value
  /// \return an optional
  template <typename T, typename E>
  constexpr auto to_optional(result<T,E>& r) -> optional<T>;
  template <typename T, typename E>
  constexpr auto to_optional(result<T,E>&& r) -> optional<T>;
  template <typename T, typename E>
  constexpr auto to_optional(const result<T,E>& r) -> optional<T>;
  template <typename T, typename E>
  constexpr auto to_optional(const result<T,E>&& r) -> optional<T>;
  /// \}

} // namespace alloy::core

//==============================================================================
// class : optional<T>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename T>
template <typename U, typename>
inline
alloy::core::optional<T>::optional(const optional<U&> other)
  : optional{}
{
  if (other.has_value()) {
    emplace(*other);
  }
}

//------------------------------------------------------------------------------
// Monadic
//------------------------------------------------------------------------------

template <typename T>
template <typename Fn>
inline constexpr
auto alloy::core::optional<T>::map(Fn&& fn)
  const& -> optional<std::invoke_result_t<Fn, const T&>>
{
  using result_type = optional<std::invoke_result_t<Fn, const T&>>;

  if (has_value()) {
    return result_type{std::invoke(std::forward<Fn>(fn), **this)};
  }
  return result_type{null};
}

template <typename T>
template <typename Fn>
inline constexpr
auto alloy::core::optional<T>::map(Fn&& fn)
  && -> optional<std::invoke_result_t<Fn, T&&>>
{
  using result_type = optional<std::invoke_result_t<Fn, T&&>>;

  if (has_value()) {
    return result_type{std::invoke(std::forward<Fn>(fn), *std::move(*this))};
  }
  return result_type{null};
}

//------------------------------------------------------------------------------

template <typename T>
template <typename Fn>
inline constexpr
auto alloy::core::optional<T>::flat_map(Fn&& fn)
  const& -> std::invoke_result_t<Fn, const T&>
{
  using result_type = std::invoke_result_t<Fn, const T&>;

  if (has_value()) {
    return std::invoke(std::forward<Fn>(fn), **this);
  }
  return result_type{null};
}

template <typename T>
template <typename Fn>
inline constexpr
auto alloy::core::optional<T>::flat_map(Fn&& fn)
  && -> std::invoke_result_t<Fn, T&&>
{
  using result_type = std::invoke_result_t<Fn, T&&>;

  if (has_value()) {
    return std::invoke(std::forward<Fn>(fn), *std::move(*this));
  }
  return result_type{null};
}

//------------------------------------------------------------------------------

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::optional<T>::and_then(U&& u)
  const -> optional<std::decay_t<U>>
{
  using result_type = optional<std::decay_t<U>>;

  return has_value() ? result_type{std::forward<U>(u)} : result_type{null};
}

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::optional<T>::and_then(std::reference_wrapper<U> ref)
  const -> optional<U&>
{
  using result_type = optional<U&>;

  return has_value() ? result_type{ref.get()} : result_type{null};
}


//==============================================================================
// class : optional<T&>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
alloy::core::optional<T&>::optional()
  noexcept
  : m_data{nullptr}
{

}

template <typename T>
template <typename U, typename>
inline constexpr
alloy::core::optional<T&>::optional(U& ref)
  noexcept
  : m_data{std::addressof(ref)}
{

}

template <typename T>
inline constexpr
alloy::core::optional<T&>::optional(null_type)
  noexcept
  : optional{}
{

}

template <typename T>
template <typename U, typename>
inline
auto alloy::core::optional<T&>::operator=(U& ref)
   noexcept -> optional&
{
  m_data = std::addressof(ref);
  return (*this);
}

template <typename T>
inline
auto alloy::core::optional<T&>::operator=(null_type)
   noexcept -> optional&
{
  reset();
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::optional<T&>::operator->()
  const noexcept -> T*
{
  ALLOY_ASSERT(m_data != nullptr);
  return m_data;
}

template <typename T>
inline constexpr
auto alloy::core::optional<T&>::operator*()
  const noexcept -> T&
{
  ALLOY_ASSERT(m_data != nullptr);
  return *m_data;
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::optional<T&>::has_value()
  const noexcept -> bool
{
  return m_data != nullptr;
}

template <typename T>
inline constexpr
alloy::core::optional<T&>::operator bool()
  const noexcept
{
  return has_value();
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::optional<T&>::value()
  const -> T&
{
  if (m_data == nullptr) {
    throw std::bad_optional_access{};
  }
  return *m_data;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::optional<T&>::reset()
  noexcept -> void
{
  m_data = nullptr;
}

//------------------------------------------------------------------------------
// Monadic
//------------------------------------------------------------------------------

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::optional<T&>::value_or(U& ref)
  const noexcept -> T&
{
  if (has_value()) {
    return *m_data;
  }
  return ref;
}

template <typename T>
template <typename Fn>
auto alloy::core::optional<T&>::map(Fn&& fn)
  const -> optional<std::invoke_result_t<Fn,T&>>
{
  using result_type = optional<std::invoke_result_t<Fn, T&>>;

  if (has_value()) {
    return result_type{std::invoke(std::forward<Fn>(fn), **this)};
  }
  return result_type{null};
}

template <typename T>
template <typename Fn>
auto alloy::core::optional<T&>::flat_map(Fn&& fn)
  const -> std::invoke_result_t<Fn, T&>
{
  using result_type = std::invoke_result_t<Fn, T&>;

  if (has_value()) {
    return std::invoke(std::forward<Fn>(fn), **this);
  }
  return result_type{null};
}

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::optional<T&>::and_then(U&& u)
  const -> optional<std::decay_t<U>>
{
  using result_type = optional<std::decay_t<U>>;

  return has_value() ? result_type{std::forward<U>(u)} : result_type{null};
}

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::optional<T&>::and_then(std::reference_wrapper<U> ref)
  const -> optional<U&>
{
  using result_type = optional<U&>;

  return has_value() ? result_type{ref.get()} : result_type{null};
}

//==============================================================================
// non-member functions : class : optional
//==============================================================================

//------------------------------------------------------------------------------
// Comparison
//------------------------------------------------------------------------------

template <typename T, typename U>
inline constexpr
auto alloy::core::operator==(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (lhs.has_value() != rhs.has_value()) { return false; }
  if (!lhs.has_value()) { return true; }
  return (*lhs) == (*rhs);
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator!=(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (lhs.has_value() != rhs.has_value()) { return true; }
  if (!lhs.has_value()) { return false; }
  return (*lhs) != (*rhs);
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (!rhs.has_value()) { return false; }
  if (!lhs.has_value()) { return true; }
  return (*lhs) < (*rhs);
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (!lhs.has_value()) { return false; }
  if (!rhs.has_value()) { return true; }
  return (*lhs) > (*rhs);
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<=(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (!lhs.has_value()) { return true; }
  if (!rhs.has_value()) { return false; }
  return (*lhs) <= (*rhs);
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>=(const optional<T>& lhs, const optional<U>& rhs)
  noexcept -> bool
{
  if (!rhs.has_value()) { return true; }
  if (!lhs.has_value()) { return false; }
  return (*lhs) >= (*rhs);
}

//------------------------------------------------------------------------------

template <typename T, typename U>
inline constexpr
auto alloy::core::operator==(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) == value) : false;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator!=(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) != value) : true;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) < value) : true;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) > value) : false;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<=(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) <= value) : true;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>=(const optional<T>& opt, const U& value)
  noexcept -> bool
{
  return opt.has_value() ? ((*opt) >= value) : false;
}

//------------------------------------------------------------------------------

template <typename T, typename U>
inline constexpr
auto alloy::core::operator==(const T& value, const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value == (*opt)) : false;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator!=(const T& value, const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value != (*opt)) : true;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<(const T& value,const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value < (*opt)) : false;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>(const T& value,const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value > (*opt)) : true;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator<=(const T& value, const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value <= (*opt)) : false;
}

template <typename T, typename U>
inline constexpr
auto alloy::core::operator>=(const T& value, const optional<U>& opt)
  noexcept -> bool
{
  return opt.has_value() ? (value >= (*opt)) : true;
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::make_optional(T&& v) -> optional<std::decay_t<T>>
{
  return optional<std::decay_t<T>>{std::forward<T>(v)};
}

template <typename T, typename...Args>
inline constexpr
auto alloy::core::make_optional(Args&&...args) -> optional<T>
{
  return optional<T>{std::in_place, std::forward<Args>(args)...};
}

template <typename T>
inline constexpr
auto alloy::core::make_optional(std::reference_wrapper<T> ref) -> optional<T&>
{
  return optional<T&>{ref.get()};
}

//------------------------------------------------------------------------------

template <typename T, typename E>
inline constexpr
auto alloy::core::to_optional(result<T,E>& r) -> optional<T>
{
  if (r) {
    return optional<T>{*r};
  }
  return null;
}

template <typename T, typename E>
inline constexpr
auto alloy::core::to_optional(result<T,E>&& r) -> optional<T>
{
  if (r) {
    return optional<T>{*std::move(r)};
  }
  return null;
}

template <typename T, typename E>
inline constexpr
auto alloy::core::to_optional(const result<T,E>& r) -> optional<T>
{
  if (r) {
    return optional<T>{*r};
  }
  return null;
}

template <typename T, typename E>
inline constexpr
auto alloy::core::to_optional(const result<T,E>&& r) -> optional<T>
{
  if (r) {
    return optional<T>{*std::move(r)};
  }
  return null;
}


#endif /* ALLOY_CORE_UTILITIES_OPTIONAL_HPP */
