// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file image.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MODEL_IMAGE_HPP
#define ALLOY_CORE_MODEL_IMAGE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstdint>      // std::uint8_t
#include <cstddef>      // std::size_t, std::byte
#include <system_error> // std::error_code, std::is_error_code_enum
#include <type_traits>  // std::true_type

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A general image abstraction
  ///
  /// An image is implemented as a contiguous array of bytes that represents
  /// a 2D graphical image.
  /////////////////////////////////////////////////////////////////////////////
  class image
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The order that pixels are stored in this image
    ///////////////////////////////////////////////////////////////////////////
    enum class pixel_format : std::uint8_t {
      rgb,
      rgba,
      bgr,
      bgra,
    };

    struct dimensions {
      std::size_t width;
      std::size_t height;
    };

    enum class error_code {
      invalid_pixel_format = 1, ///< Pixel format is not correct
      bad_width = 2,            ///< Width specified as 0
      bad_height = 3,           ///< Height specified as 0
      incorrect_dimensions = 4, ///< Dimensions do not match buffer size
    };

    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes an image from a byte buffer
    ///
    /// \pre \p buffer must be of n * d.width * d.height size, where n is the
    ///         number of bytes specified in the pixel_format
    /// \pre `d.width` must be greater than 0
    /// \pre `d.height` must be greater than 0
    ///
    /// \param buffer the underlying byte buffer
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static auto make_image_from_buffer(
      vector<std::byte> buffer,
      dimensions d,
      pixel_format format
    ) -> result<image,std::error_code>;

    /// \brief Makes an image with a solid color
    ///
    /// \pre \p buffer must be of n * d.width * d.height size, where n is the
    ///         number of bytes specified in the pixel_format
    /// \pre `d.width` must be greater than 0
    /// \pre `d.height` must be greater than 0
    ///
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    /// \param background the color to set the background of the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static auto make_blank_image(
      dimensions d,
      pixel_format format,
      color background
    ) -> result<image,std::error_code>;

    /// \brief Makes an image with a solid color
    ///
    /// \pre \p buffer must be of n * d.width * d.height size, where n is the
    ///         number of bytes specified in the pixel_format
    /// \pre `d.width` must be greater than 0
    /// \pre `d.height` must be greater than 0
    ///
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    /// \param background the color to set the background of the image
    /// \param alloc the allocator to use for constructing the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static auto make_blank_image(
      dimensions d,
      pixel_format format,
      color background,
      allocator alloc
    ) -> result<image,std::error_code>;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an image of 0 size
    ALLOY_CORE_API
    image() noexcept;
    image(image&& other) noexcept = default;
    image(const image&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(image&& other) noexcept -> image& = default;
    auto operator=(const image&) -> image& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the pixel at the image coordinate (x,y)
    ///
    /// \pre \p x must be between [0, width())
    /// \pre \p y must be between [0, height())
    ///
    /// \param x the x position
    /// \param y the y position
    /// \param c the color
    ALLOY_CORE_API
    auto set_pixel(std::size_t x, std::size_t y, color c) noexcept -> void;

    //--------------------------------------------------------------------------
    // Accessors
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the pixel at the image coordinate (x,y)
    ///
    /// \pre \p x must be between [0, width())
    /// \pre \p y must be between [0, height())
    ///
    /// \param x the x coordinate
    /// \param y the y coordinate
    /// \return the color at image coordinate (x,y)
    ALLOY_CORE_API
    auto get_pixel(std::size_t x, std::size_t y) const noexcept -> color;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Returns a copy of this image
    ///
    /// \return the copy of the image
    ALLOY_CORE_API
    auto copy() const -> image;

    /// \brief Returns a copy of this image allocated using \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the new image
    ALLOY_CORE_API
    auto copy_with(allocator alloc) const -> image;

    //--------------------------------------------------------------------------

    /// \brief Returns a pointer to the underlying image data
    ///
    /// \return a pointer to the underlying image data
    auto data() const noexcept -> const std::byte*;

    /// \brief Gets the width of this image
    ///
    /// \return the width of this image
    auto width() const noexcept -> std::size_t;

    /// \brief Gets the height of this image
    ///
    /// \return the height of this image
    auto height() const noexcept -> std::size_t;

    /// \brief Gets the pixel format of this image
    ///
    /// \return the pixel format of this image
    auto format() const noexcept -> pixel_format;

    //--------------------------------------------------------------------------
    // Private : Constructors
    //--------------------------------------------------------------------------
  private:

    /// \brief Fully constructs the underlying image
    image(vector<std::byte> bytes, dimensions d, pixel_format format) noexcept;

    //--------------------------------------------------------------------------
    // Private
    //--------------------------------------------------------------------------
  private:

    vector<std::byte> m_bytes;
    std::size_t m_width;
    std::size_t m_height;
    pixel_format m_format;
  };

  //===========================================================================
  // non-member functions : enum : image::error_code
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Makes an error code from an image error_code
  ///
  /// \param ec the error code
  /// \return the error code
  ALLOY_CORE_API
  auto make_error_code(image::error_code ec) noexcept -> std::error_code;

} // namespace alloy::core

namespace std {
  template <>
  struct is_error_code_enum<::alloy::core::image::error_code>
    : std::true_type{};
} // namespace error_code

//=============================================================================
// inline definitions : class : image
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
auto alloy::core::image::data() const noexcept -> const std::byte*
{
  return m_bytes.data();
}


inline
auto alloy::core::image::width() const noexcept -> std::size_t
{
  return m_width;
}


inline
auto alloy::core::image::height() const noexcept -> std::size_t
{
  return m_height;
}


inline
auto alloy::core::image::format() const noexcept -> pixel_format
{
  return m_format;
}

#endif /* ALLOY_CORE_MODEL_IMAGE_HPP */

