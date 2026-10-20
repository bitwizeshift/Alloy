// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file interpolated_transform.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MODEL_INTERPOLATED_TRANSFORM_HPP
#define ALLOY_CORE_MODEL_INTERPOLATED_TRANSFORM_HPP

#include "alloy/core/model/transform.hpp"
#include "alloy/core/model/camera.hpp"
#include "alloy/core/model/projection.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/precision.hpp"

#include <array> // std::array

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A simple wrapper around a Transform type that ensures that it is
  ///        both lazily computed, and also interpolated between the previous
  ///        and current state
  ///
  /// By default, 3 aliases are added for this type for the `camera`,
  /// `projection`, and (model) `transform` classes.
  ///
  /// \tparam Transform the underlying transform
  /////////////////////////////////////////////////////////////////////////////
  template <typename Transform>
  class interpolated_transform
  {
    class indirect_pointer;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs the interpolated_transform if the underlying
    ///        Transform is default-constructible
    interpolated_transform() noexcept = default;

    /// \brief Constructs this interpolated_transform by copying the contents of
    ///        \p transform
    ///
    /// \param transform the transformation to copy
    interpolated_transform(const Transform& transform) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    interpolated_transform(const interpolated_transform& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    /// \return reference to (*this)
    auto operator=(const interpolated_transform& other) noexcept -> interpolated_transform& = default;

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
    auto operator->() noexcept -> indirect_pointer;

    /// \brief Gets a const pointer to the underlying transform
    ///
    /// This will *not* set the dirty bit, since the underlying transform will
    /// not be modified.
    ///
    /// The recommended way to access this, if this interpolated_transform is non-const,
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

    /// \brief Updates the interpolated state of the transform
    ///
    /// If there is no change to this transformation, then \p out will not be
    /// updated
    ///
    /// \param out a pointer to the matrix to update
    /// \param alpha the percent between the previous and current state [0,1]
    auto update_matrix(not_null<matrix4*> out, real alpha) noexcept -> void;

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

    std::array<Transform,2u> m_transforms;
    bool m_is_dirty;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A pointer-like type returned by `interpolated_transform`
  ///        to store the result of the `operator->` call after mutating the
  ///        transform
  //////////////////////////////////////////////////////////////////////////////
  template <typename Transform>
  class interpolated_transform<Transform>::indirect_pointer final
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    explicit indirect_pointer(Transform* transforms);
    indirect_pointer(indirect_pointer&&) = delete;
    indirect_pointer(const indirect_pointer&) = delete;

    //--------------------------------------------------------------------------

    ~indirect_pointer();

    //--------------------------------------------------------------------------

    auto operator=(indirect_pointer&&) -> indirect_pointer& = delete;
    auto operator=(const indirect_pointer&) -> indirect_pointer& = delete;

    //--------------------------------------------------------------------------
    // Indirection
    //--------------------------------------------------------------------------
  public:

    auto operator->() && noexcept -> Transform*;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    Transform* m_transforms;
  };

  //===========================================================================
  // aliases : class : interpolated_transform
  //===========================================================================

  using interpolated_model_transform = interpolated_transform<transform>;
  using interpolated_camera          = interpolated_transform<camera>;

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template<typename Transform>
alloy::core::interpolated_transform<Transform>::interpolated_transform(const Transform& transform)
  noexcept
  : m_transforms{transform, transform},
    m_is_dirty{false}
{

}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template<typename Transform>
inline
auto alloy::core::interpolated_transform<Transform>::operator->()
  noexcept -> indirect_pointer
{
  m_is_dirty = true;
  return indirect_pointer{m_transforms.data()};
}

template<typename Transform>
inline
auto alloy::core::interpolated_transform<Transform>::operator->()
  const noexcept -> const Transform*
{
  return &m_transforms[1];
}

//-----------------------------------------------------------------------------
// Extraction
//-----------------------------------------------------------------------------

template <typename Transform>
inline
auto alloy::core::interpolated_transform<Transform>::update_matrix(
  not_null<matrix4*> out,
  real alpha
) noexcept -> void
{
  if (!m_is_dirty) {
    return;
  }
  auto result = interpolation::linear(m_transforms[0], m_transforms[1], alpha);
  result.extract_matrix(out);
  m_is_dirty = false;
}

template <typename Transform>
inline
auto alloy::core::interpolated_transform<Transform>::update_matrix(
  not_null<matrix4*> out
) noexcept -> void
{
  if (!m_is_dirty) {
    return;
  }
  m_transforms[1].extract_matrix(out);
  m_is_dirty = false;
}

//==============================================================================
// class : interpolated_transform<Transform>::indirect_pointer
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor
//------------------------------------------------------------------------------

template <typename Transform>
inline
alloy::core::interpolated_transform<Transform>::indirect_pointer::indirect_pointer(
  Transform* transforms
) : m_transforms{transforms}
{

}

template <typename Transform>
inline
alloy::core::interpolated_transform<Transform>::indirect_pointer::~indirect_pointer()
{
  m_transforms[0] = m_transforms[1];
}

//------------------------------------------------------------------------------
// Indirection
//------------------------------------------------------------------------------

template <typename Transform>
auto alloy::core::interpolated_transform<Transform>::indirect_pointer::operator->()
  && noexcept  -> Transform*
{
  return &m_transforms[1];
}

#endif /* ALLOY_CORE_MODEL_INTERPOLATED_TRANSFORM_HPP */
