// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file image_loader.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP
#define ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/utilities/result.hpp"
#include "alloy/core/model/image.hpp"
#include "alloy/io/filesystem/file.hpp"

#include <system_error> // std::error_code

namespace alloy::engine {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An abstraction for loading images from a file
  ///
  /// This abstraction is opaque in terms of what the underlying file-type being
  /// loaded is.
  //////////////////////////////////////////////////////////////////////////////
  class image_loader
  {
  public:

    virtual ~image_loader() = default;

    /// \brief Attempts to load an image from the specified \p file
    ///
    /// \param file the file to load from
    /// \return the image on success
    virtual auto load(io::file& file)
      -> core::result<core::image,std::error_code> = 0;
  };

} // namespace alloy::engine

#endif /* ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP */
