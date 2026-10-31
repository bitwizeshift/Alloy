// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

///////////////////////////////////////////////////////////////////////////////
/// \file cached_transform.hpp
///
/// \brief This header defines a wrapper that caches the matrix of a
///        transformation until the transformation changes
///////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MODEL_CACHED_TRANSFORM_HPP
#define ALLOY_CORE_MODEL_CACHED_TRANSFORM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/model/camera.hpp"
#include "alloy/core/model/projection.hpp"
#include "alloy/core/model/transform.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <concepts>    // std::constructible_from, std::copy_constructible
#include <type_traits> // std::is_nothrow_constructible_v
#include <utility>     // std::in_place_t, std::move, std::forward

namespace alloy::core {

  //===========================================================================
  // concept : matrix_transform
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A transformation that can be converted to a `matrix4`
  ///
  /// The transformation writes its matrix through `extract_matrix`, which
  /// takes a non-null pointer to the `matrix4` to write.
  ///
  /// \tparam T the transformation type
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  concept matrix_transform = requires(const T& t, not_null<matrix4*> out) {
    { t.extract_matrix(out) } noexcept;
  };

  //===========================================================================
  // class : cached_transform
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A wrapper around a transformation that caches its matrix, and
  ///        only recomputes it after the transformation has been modified
  ///
  /// Transformations that rarely change, such as those of static objects or
  /// of a projection, compute their matrix once and then reuse it on every
  /// later frame.
  ///
  /// The transformation is modified through the non-const `operator->` or
  /// `operator*`, which mark the cached matrix as stale:
  ///
  /// \code
  /// auto camera = cached_camera{};
  /// camera->translate_world_x(1);       // marks the matrix as stale
  /// const auto& view = camera.matrix(); // recomputes the matrix once
  /// \endcode
  ///
  /// \tparam Transform the underlying transform
  /////////////////////////////////////////////////////////////////////////////
  template <matrix_transform Transform>
  class cached_transform
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs the underlying transform
    cached_transform()
      requires std::default_initializable<Transform> = default;

    /// \brief Constructs this cached_transform by copying \p transform
    ///
    /// \param transform the transformation to copy
    explicit cached_transform(const Transform& transform)
      noexcept(std::is_nothrow_copy_constructible_v<Transform>)
      requires std::copy_constructible<Transform>;

    /// \brief Constructs this cached_transform by moving \p transform
    ///
    /// \param transform the transformation to move
    explicit cached_transform(Transform&& transform)
      noexcept(std::is_nothrow_move_constructible_v<Transform>);

    /// \brief Constructs the underlying transform in place from \p args
    ///
    /// \param args the arguments to construct the transformation with
    template <typename...Args>
      requires std::constructible_from<Transform, Args...>
    explicit cached_transform(std::in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<Transform, Args...>);

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Accesses the underlying transform to modify it
    ///
    /// This marks the cached matrix as stale, so the next call to `matrix`
    /// recomputes it. The result must not be used to modify the transform
    /// after that call.
    ///
    /// \return the underlying transform
    auto operator->() noexcept -> Transform*;
    auto operator*() & noexcept -> Transform&;
    /// \}

    /// \brief Moves the underlying transform out of this cached_transform
    ///
    /// \return the underlying transform
    auto operator*() && noexcept -> Transform&&;

    /// \{
    /// \brief Accesses the underlying transform without modifying it
    ///
    /// This does not mark the cached matrix as stale.
    ///
    /// \return the underlying transform
    auto operator->() const noexcept -> const Transform*;
    auto operator*() const & noexcept -> const Transform&;
    auto value() const noexcept -> const Transform&;
    /// \}

    //-------------------------------------------------------------------------
    // Conversion
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the matrix of the underlying transform
    ///
    /// The matrix is only recomputed if the transform has been modified since
    /// the matrix was last computed.
    ///
    /// \return reference to the matrix of the transform
    auto matrix() noexcept -> const matrix4&;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    Transform m_transform;
    matrix4 m_matrix;
    bool m_is_stale = true;
  };

  //===========================================================================
  // aliases : class : cached_transform
  //===========================================================================

  using cached_model_transform = cached_transform<transform>;
  using cached_camera          = cached_transform<camera>;
  using cached_projection      = cached_transform<projection>;

} // namespace alloy::core

//=============================================================================
// definitions : class : cached_transform
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <alloy::core::matrix_transform Transform>
inline
alloy::core::cached_transform<Transform>::cached_transform(
  const Transform& transform
) noexcept(std::is_nothrow_copy_constructible_v<Transform>)
  requires std::copy_constructible<Transform>
  : m_transform{transform}
{

}

template <alloy::core::matrix_transform Transform>
inline
alloy::core::cached_transform<Transform>::cached_transform(
  Transform&& transform
) noexcept(std::is_nothrow_move_constructible_v<Transform>)
  : m_transform{std::move(transform)}
{

}

template <alloy::core::matrix_transform Transform>
template <typename...Args>
  requires std::constructible_from<Transform, Args...>
inline
alloy::core::cached_transform<Transform>::cached_transform(
  std::in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<Transform, Args...>)
  // Parentheses, so that a list-initializable transform is not constructed
  // from an initializer list instead.
  : m_transform(std::forward<Args>(args)...)
{

}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::operator->()
  noexcept -> Transform*
{
  m_is_stale = true;
  return &m_transform;
}

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::operator*()
  & noexcept -> Transform&
{
  m_is_stale = true;
  return m_transform;
}

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::operator*()
  && noexcept -> Transform&&
{
  return std::move(m_transform);
}

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::operator->()
  const noexcept -> const Transform*
{
  return &m_transform;
}

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::operator*()
  const & noexcept -> const Transform&
{
  return m_transform;
}

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::value()
  const noexcept -> const Transform&
{
  return m_transform;
}

//-----------------------------------------------------------------------------
// Conversion
//-----------------------------------------------------------------------------

template <alloy::core::matrix_transform Transform>
inline
auto alloy::core::cached_transform<Transform>::matrix()
  noexcept -> const matrix4&
{
  if (m_is_stale) {
    m_transform.extract_matrix(&m_matrix);
    m_is_stale = false;
  }
  return m_matrix;
}

#endif /* ALLOY_CORE_MODEL_CACHED_TRANSFORM_HPP */
