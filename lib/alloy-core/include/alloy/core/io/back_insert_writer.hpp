// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file back_insert_writer.hpp
///
/// \brief This header contains the definition of the back_insert_writer, a
///        writer that appends bytes to the back of a sequence container
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_BACK_INSERT_WRITER_HPP
#define ALLOY_CORE_IO_BACK_INSERT_WRITER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/writer.hpp"
#include "alloy/core/utilities/delegate.hpp"
#include "alloy/core/utilities/result.hpp"

#include <concepts>     // std::integral, std::same_as
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  namespace detail {

    /// \brief True when `T` is a single byte: either `std::byte`, or an
    ///        integral type other than `bool` that is one byte in size
    template <typename T>
    concept byte_like =
      std::same_as<std::remove_cv_t<T>, std::byte> ||
      (std::integral<std::remove_cv_t<T>> && !std::same_as<std::remove_cv_t<T>, bool> && sizeof(T) == 1u);

    /// \brief True when `Container` is a sequence container of byte_like
    ///        values that can be appended to with `push_back`
    template <typename Container>
    concept byte_back_insertable =
      byte_like<typename Container::value_type> &&
      requires(Container& c, typename Container::value_type v) {
        c.push_back(v);
      };

  } // namespace detail

  //===========================================================================
  // class : back_insert_writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A writer that appends every byte written to it to the back of a
  ///        sequence container
  ///
  /// Each write appends every byte of the buffer with the container's
  /// `push_back`, so a back_insert_writer never fails and never accepts fewer
  /// bytes than it is given.
  ///
  /// \note The back_insert_writer does not own the container it appends to.
  ///       Care must be taken to ensure that the back_insert_writer does not
  ///       outlive it.
  ///
  /// \note An exception thrown by the container's `push_back`, such as on
  ///       allocation failure, cannot propagate out of a write, and
  ///       terminates instead.
  /////////////////////////////////////////////////////////////////////////////
  class back_insert_writer final : public writer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a back_insert_writer that appends to \p container
    ///
    /// \pre \p container must outlive the back_insert_writer
    ///
    /// \param container the container to append to
    template <typename Container>
      requires (alloy::core::detail::byte_back_insertable<Container>)
    explicit back_insert_writer(Container& container) noexcept;
    back_insert_writer(back_insert_writer&&) noexcept = default;
    back_insert_writer(const back_insert_writer&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(back_insert_writer&&) noexcept
      -> back_insert_writer& = default;
    auto operator=(const back_insert_writer&) noexcept
      -> back_insert_writer& = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Static Functions
    //-------------------------------------------------------------------------
  private:

    /// \brief Appends \p b to the back of \p container
    ///
    /// \param container the container to append to
    /// \param b the byte to append
    template <typename Container>
    static auto push_back(Container* container, std::byte b) -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    delegate<void(std::byte)> m_push_back;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : back_insert_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename Container>
  requires (alloy::core::detail::byte_back_insertable<Container>)
ALLOY_FORCE_INLINE
alloy::core::back_insert_writer::back_insert_writer(Container& container)
  noexcept
  : m_push_back{
      delegate<void(std::byte)>::make<&push_back<Container>>(&container)
    }
{

}

//-----------------------------------------------------------------------------
// Virtual Hooks : Writing
//-----------------------------------------------------------------------------

// The container's push_back may throw on allocation failure, which is treated
// as fatal.
inline
// NOLINTNEXTLINE(bugprone-exception-escape)
auto alloy::core::back_insert_writer::do_write(
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  for (const auto b : buffer) {
    m_push_back(b);
  }
  return buffer.size();
}

//-----------------------------------------------------------------------------
// Private Static Functions
//-----------------------------------------------------------------------------

template <typename Container>
ALLOY_FORCE_INLINE
auto alloy::core::back_insert_writer::push_back(
  Container* container,
  std::byte b
) -> void
{
  using value_type = typename Container::value_type;

  container->push_back(static_cast<value_type>(b));
}

#endif /* ALLOY_CORE_IO_BACK_INSERT_WRITER_HPP */
