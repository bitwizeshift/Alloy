/*****************************************************************************
 * \file image.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MODEL_IMAGE_HPP
#define ALLOY_CORE_MODEL_IMAGE_HPP

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
    /// \pre \c d.width must be greater than 0
    /// \pre \c d.height must be greater than 0
    ///
    /// \param buffer the underlying byte buffer
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static result<image,std::error_code> make_image_from_buffer(vector<std::byte> buffer,
                                                  dimensions d,
                                                  pixel_format format);

    /// \brief Makes an image with a solid color
    ///
    /// \pre \p buffer must be of n * d.width * d.height size, where n is the
    ///         number of bytes specified in the pixel_format
    /// \pre \c d.width must be greater than 0
    /// \pre \c d.height must be greater than 0
    ///
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    /// \param background the color to set the background of the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static result<image,std::error_code> make_blank_image(dimensions d,
                                            pixel_format format,
                                            color background);

    /// \brief Makes an image with a solid color
    ///
    /// \pre \p buffer must be of n * d.width * d.height size, where n is the
    ///         number of bytes specified in the pixel_format
    /// \pre \c d.width must be greater than 0
    /// \pre \c d.height must be greater than 0
    ///
    /// \param d the dimensions of the image
    /// \param format the pixel format of the image
    /// \param background the color to set the background of the image
    /// \param alloc the allocator to use for constructing the image
    ///
    /// \return an image on success, an error_code on failure
    ALLOY_CORE_API
    static result<image,std::error_code> make_blank_image(dimensions d,
                                            pixel_format format,
                                            color background,
                                            allocator alloc);

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

    image& operator=(image&& other) noexcept = default;
    image& operator=(const image&) = delete;

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
    void set_pixel(std::size_t x, std::size_t y, color c) noexcept;

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
    color get_pixel(std::size_t x, std::size_t y) const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Returns a copy of this image
    ///
    /// \return the copy of the image
    ALLOY_CORE_API
    image copy() const;

    /// \brief Returns a copy of this image allocated using \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the new image
    ALLOY_CORE_API
    image copy(allocator alloc) const;

    //--------------------------------------------------------------------------

    /// \brief Returns a pointer to the underlying image data
    ///
    /// \return a pointer to the underlying image data
    const std::byte* data() const noexcept;

    /// \brief Gets the width of this image
    ///
    /// \return the width of this image
    std::size_t width() const noexcept;

    /// \brief Gets the height of this image
    ///
    /// \return the height of this image
    std::size_t height() const noexcept;

    /// \brief Gets the pixel format of this image
    ///
    /// \return the pixel format of this image
    pixel_format format() const noexcept;

    //--------------------------------------------------------------------------
    // Private : Constructors
    //--------------------------------------------------------------------------
  private:

    /// \brief Fully constructs the underlying image
    image(vector<std::byte> bytes,
          dimensions d,
          pixel_format format) noexcept;

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
  std::error_code make_error_code(image::error_code ec) noexcept;

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

inline const std::byte* alloy::core::image::data()
  const noexcept
{
  return m_bytes.data();
}


inline std::size_t alloy::core::image::width()
  const noexcept
{
  return m_width;
}


inline std::size_t alloy::core::image::height()
  const noexcept
{
  return m_height;
}


inline alloy::core::image::pixel_format alloy::core::image::format()
  const noexcept
{
  return m_format;
}

#endif /* ALLOY_CORE_MODEL_IMAGE_HPP */
