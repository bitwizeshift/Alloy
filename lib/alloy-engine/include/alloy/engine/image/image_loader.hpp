////////////////////////////////////////////////////////////////////////////////
/// \file image_loader.hpp
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

#ifndef ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP
#define ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP

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
    virtual auto load(io::file& file) -> core::result<core::image,std::error_code> = 0;
  };

} // namespace alloy::engine

#endif /* ALLOY_ENGINE_IMAGE_IMAGE_LOADER_HPP */
