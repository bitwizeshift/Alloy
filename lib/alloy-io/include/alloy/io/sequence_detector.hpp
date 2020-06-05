////////////////////////////////////////////////////////////////////////////////
/// \file sequence_detector.hpp
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

#ifndef ALLOY_IO_SEQUENCE_DETECTOR_HPP
#define ALLOY_IO_SEQUENCE_DETECTOR_HPP

#include <alloy/core/assert.hpp>
#include <alloy/core/containers/vector.hpp>

#include <type_traits> // std::is_same, std::is_nothrow_copy_constructible, etc

namespace alloy::io {

  //===========================================================================
  // definition : class : sequence_detector
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A type used for detecting sequences of values
  ///
  /// This type may be used to determine input or event sequences that occur in
  /// an expected order.
  ///
  /// To test if a sequence is detected, sequence_detector::completed will
  /// return true. If a sequence has been detected, sequence_detector::reset
  ///
  /// \tparam T the value type
  /// \tparam Container the container to store the sequence. Must satisfy
  ///                   the SequenceContainer concept.
  /////////////////////////////////////////////////////////////////////////////
  template <typename T, typename Container = core::vector<T>>
  class sequence_detector
  {
    static_assert(
      std::is_same<T,typename Container::value_type>::value,
      "T must be the same as the Container's value_type"
    );

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = typename Container::value_type;
    using container  = Container;
    using size_type  = typename Container::size_type;

    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    sequence_detector() = delete;

    /// \brief Constructs this sequence_detector from a \p container of the
    ///        desired sequence
    ///
    /// \param container the container to copy
    constexpr sequence_detector(const container& sequence)
      noexcept(std::is_nothrow_copy_constructible<Container>::value);

    /// \brief Constructs this sequence_detector from a \p container of the
    ///        desired sequence
    ///
    /// \param container the container to move
    constexpr sequence_detector(container&& sequence)
      noexcept(std::is_nothrow_move_constructible<Container>::value);

    /// \brief Constructs this sequence_detector by moving the sequence from
    ///        \p other
    ///
    /// \param other the other detector to move
    constexpr sequence_detector(sequence_detector&& other) = default;
    sequence_detector(const sequence_detector&) = delete;

    //-------------------------------------------------------------------------

    sequence_detector& operator=(sequence_detector&&) = delete;
    sequence_detector& operator=(const sequence_detector&) = delete;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Tests if \p value is the next value in the sequence
    ///
    /// A completed sequence will always return false until reset.
    ///
    /// \param value the value to test
    /// \return true if \p value was the next value in the sequence
    constexpr bool test(const value_type& value) noexcept;

    /// \brief Resets this sequence
    ///
    /// \note reset must be called after a sequence is completed in order to
    ///       detect a new sequence
    constexpr void reset() noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Checks whether this sequence has been started
    ///
    /// A sequence is considered 'started' if at least one element has been
    /// matched.
    ///
    /// \return true if this sequence is started
    constexpr bool started() const noexcept;

    /// \brief Checks whether this sequence has been completed successfully
    ///
    /// A sequence is considered 'completed' if all entries in the sequence has
    /// been matched successfully.
    ///
    /// \return true if this sequence is completed
    constexpr bool completed() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    size_type m_index;
    container m_sequence;
  };

  template <typename Container>
  sequence_detector(Container&&) -> sequence_detector<
    typename std::decay_t<Container>::value_type,
    std::decay_t<Container>
  >;

} // namespace alloy::io

//=============================================================================
// definition : class : sequence_detector
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename T, typename Container>
inline constexpr
alloy::io::sequence_detector<T,Container>::sequence_detector(const container& sequence)
  noexcept(std::is_nothrow_copy_constructible<Container>::value)
  : m_index{0u},
    m_sequence{sequence}
{

}

template <typename T, typename Container>
inline constexpr
alloy::io::sequence_detector<T,Container>::sequence_detector(container&& sequence)
  noexcept(std::is_nothrow_move_constructible<Container>::value)
  : m_index{0u},
    m_sequence{std::move(sequence)}
{

}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T, typename Container>
inline constexpr
bool alloy::io::sequence_detector<T,Container>::test(const value_type& value)
  noexcept
{
  ALLOY_ASSERT(!m_sequence.empty(), "Sequence cannnot be empty");

  if (completed()) {
    return false;
  }

  // This case should never occur
  ALLOY_ASSERT(m_index < m_sequence.size());

  if (m_sequence[m_index] != value) {
    m_index = 0u;
    return false;
  }
  m_index++;
  return true;
}

template <typename T, typename Container>
inline constexpr
void alloy::io::sequence_detector<T,Container>::reset()
  noexcept
{
  m_index = 0u;
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T, typename Container>
inline constexpr
bool alloy::io::sequence_detector<T,Container>::started()
  const noexcept
{
  return m_index != 0u;
}

template <typename T, typename Container>
inline constexpr
bool alloy::io::sequence_detector<T,Container>::completed()
  const noexcept
{
  return m_index == m_sequence.size();
}

#endif /* ALLOY_IO_SEQUENCE_DETECTOR_HPP */
