///////////////////////////////////////////////////////////////////////////////
/// \file lazy_transform.hpp
///
/// \todo(Bitwize): Document this
///////////////////////////////////////////////////////////////////////////////

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
#ifndef ALLOY_CORE_MODEL_LAZY_TRANSFORM_HPP
#define ALLOY_CORE_MODEL_LAZY_TRANSFORM_HPP

#include "alloy/core/model/transform.hpp"
#include "alloy/core/model/camera.hpp"
#include "alloy/core/model/projection.hpp"

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A simple wrapper around a Transform type that ensures that
  ///        computing the underlying matrix is only done if there has been
  ///        a change to the transformation
  ///
  /// This may be used to ensure that matrices that don't change often are
  /// only evaluated when changed -- which prevents the need for redundant
  /// recomputations per-frame.
  ///
  /// This works with any transformation type that defines an 'extract_matrix'
  /// member function that takes a non-const pointer to a `matrix4`.
  ///
  /// By default, 3 aliases are added for this type for the `camera`,
  /// `projection`, and (model) `transform` classes.
  ///
  /// \tparam Transform the underlying transform
  /////////////////////////////////////////////////////////////////////////////
  template <typename Transform>
  class lazy_transform
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs the lazy_transform if the underlying Transform
    ///        is default-constructible
    lazy_transform() noexcept = default;

    /// \brief Constructs this lazy_transform by copying the contents of
    ///        \p transform
    ///
    /// \param transform the transformation to copy
    lazy_transform(const Transform& transform) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    lazy_transform(const lazy_transform& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    /// \return reference to (*this)
    auto operator=(const lazy_transform& other) noexcept -> lazy_transform& = default;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the underlying transform
    ///
    /// This will automatically set the dirty bit, which forces a transform
    /// change with `update_matrix`
    ///
    /// \return a pointer to the transform
    auto operator->() noexcept -> Transform*;

    /// \brief Gets a const pointer to the underlying transform
    ///
    /// This will *not* set the dirty bit, since the underlying transform will
    /// not be modified.
    ///
    /// The recommended way to access this, if this lazy_transform is non-const,
    /// is to use as_const:
    /// \code
    /// const auto translation = std::as_const(transform)->translation();
    /// \endcode
    ///
    /// \return a pointer to the transform
    auto operator->() const noexcept -> const Transform*;

    //-------------------------------------------------------------------------
    // Extraction
    //-------------------------------------------------------------------------
  public:

    /// \brief Updates the state of the matrix by storing the result in \p out
    ///
    /// If there is no change to this transformation, then \p out will not be
    /// updated
    ///
    /// \param out a pointer to the matrix to update
    auto update_matrix(not_null<matrix4*> out) noexcept -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    Transform m_transform;
    bool m_is_dirty = false;
  };

  //===========================================================================
  // aliases : class : lazy_transform
  //===========================================================================

  using lazy_model_transform = lazy_transform<transform>;
  using lazy_camera          = lazy_transform<camera>;
  using lazy_projection      = lazy_transform<projection>;

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template<typename Transform>
alloy::core::lazy_transform<Transform>::lazy_transform(const Transform& transform)
  noexcept
  : m_transform{transform}
{

}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template<typename Transform>
inline
auto alloy::core::lazy_transform<Transform>::operator->()
  noexcept -> Transform*
{
  m_is_dirty = true;
  return &m_transform;
}

template<typename Transform>
inline
auto alloy::core::lazy_transform<Transform>::operator->()
  const noexcept -> const Transform*
{
  return &m_transform;
}

//-----------------------------------------------------------------------------
// Extraction
//-----------------------------------------------------------------------------

template <typename Transform>
inline
auto alloy::core::lazy_transform<Transform>::update_matrix(not_null<matrix4*> out)
  noexcept -> void
{
  if (!m_is_dirty) {
    return;
  }
  m_transform.extract_matrix(out);
  m_is_dirty = false;
}

#endif /* ALLOY_CORE_MODEL_LAZY_TRANSFORM_HPP */
