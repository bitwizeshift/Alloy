// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file tga_image_loader.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP
#define ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP

#ifdef _MSC_VER
# pragma once
#endif

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
    auto load(platform::file& file)
      -> core::result<core::image,std::error_code> override;

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
  auto make_error_code(tga_image_loader::error error)
    noexcept -> std::error_code;

} // namespace alloy::engine

namespace std {

  template <>
  struct is_error_code_enum<alloy::engine::tga_image_loader::error> : std::true_type{};

} // namespace std

#endif /* ALLOY_ENGINE_IMAGE_TGA_IMAGE_LOADER_HPP */
