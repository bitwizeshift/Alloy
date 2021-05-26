////////////////////////////////////////////////////////////////////////////////
/// \file tga_image_loader.hpp
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

#ifndef ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP
#define ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP

#include "alloy/engine/image/image_loader.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/result.hpp"

#include <system_error> // std::is_error_code_enum
#include <type_traits>  // std::true_type

namespace alloy::engine {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An image loader for TGA file types
  ///
  /// This loader is written in terms of the third-party TGA library by asprite.
  ///
  /// TODO(bitwize): replace with a homebrew solution?
  //////////////////////////////////////////////////////////////////////////////
  class tga_image_loader : public image_loader
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    enum class error {
      none,
      invalid_tga_header,
      bad_image_data,
    };

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this TGA loader
    tga_image_loader() = default;

    /// \brief Constructs a loader that will allocate all image data to \p alloc
    ///
    /// \param alloc the allocator to use for image data
    explicit tga_image_loader(core::allocator alloc) noexcept;

    //--------------------------------------------------------------------------
    // Loading
    //--------------------------------------------------------------------------
  public:

    /// \brief Loads a TGA image from the specified \p file
    ///
    /// \param file the file to read
    /// \return the image on success
    auto load(io::file& file) -> core::result<core::image,std::error_code> override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::allocator m_allocator;
  };

  /// \brief Converts \p error to an error_code
  ///
  /// \param error the error to convert
  /// \return an error_code
  auto make_error_code(tga_image_loader::error error) noexcept -> std::error_code;

} // namespace alloy::engine

namespace std {

  template <>
  struct is_error_code_enum<alloy::engine::tga_image_loader::error> : std::true_type{};

} // namespace std

#endif /* ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP */
