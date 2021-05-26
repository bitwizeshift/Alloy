////////////////////////////////////////////////////////////////////////////////
/// \file image_manager.hpp
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

#ifndef ALLOY_ENGINE_IMAGE_IMAGE_MANAGER_HPP
#define ALLOY_ENGINE_IMAGE_IMAGE_MANAGER_HPP

#include "alloy/engine/image/image_loader.hpp"

#include "alloy/core/utilities/result.hpp"
#include "alloy/core/model/image.hpp"
#include "alloy/core/assert.hpp"

#include <unordered_map> // std::unordered_map
#include <string> // std::string
#include <memory> // std::unique_ptr
#include <utility> // std::forward
#include <system_error> // std::error_code
#include <type_traits> // std::true_type

namespace alloy::engine {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A manager for different image loaders
  ///
  /// This allows for selecting the appropriate image-loader from an installed
  /// string representation of a file-type.
  ///
  /// TODO(bitwize): Use Alloy containers instead of std
  //////////////////////////////////////////////////////////////////////////////
  class image_manager
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    enum class error
    {
      none,                    ///< No error occurred
      file_type_not_supported, ///< The specified file-type is not supported
    };

    //--------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs the image manager
    image_manager();

    //--------------------------------------------------------------------------
    // Installation
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs and stores a loader inside this image_manager
    ///
    /// \tparam Loader the concrete type of the loader to create
    /// \param file_type the type of file to associate with the loader
    /// \param args the arguments to forward to the loader's constructor
    /// \return a reference to the constructed loader.
    template <typename Loader, typename...Args>
    auto emplace_loader(std::string file_type, Args&&...args) -> Loader&;

    //--------------------------------------------------------------------------
    // Image Loading
    //--------------------------------------------------------------------------
  public:

    /// \brief Attempts to load an image from the given \p file with the
    ///        expected \p file_type
    ///
    /// \param file_type the type of file to load
    /// \param file the file to load from
    /// \return the image, on success
    auto load(const std::string& file_type, io::file& file)
      -> core::result<core::image,std::error_code>;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using loader_map = std::unordered_map<std::string,std::unique_ptr<image_loader>>;

    loader_map m_loaders;
  };

  /// \brief Converts \p error to an error_code
  ///
  /// \param error the error to convert
  /// \return an error_code
  auto make_error_code(image_manager::error error) noexcept -> std::error_code;

} // namespace alloy::engine

namespace std {

  template <>
  struct is_error_code_enum<alloy::engine::image_manager::error> : std::true_type{};

} // namespace std

template <typename Loader, typename...Args>
inline
auto alloy::engine::image_manager::emplace_loader(std::string file_type, Args&&...args)
  -> Loader&
{
  auto result = m_loaders.try_emplace(
    std::move(file_type),
    std::make_unique<Loader>(std::forward<Args>(args)...)
  );
  ALLOY_ASSERT(result.second, "Only one loader per file type is allowed");
  return static_cast<Loader&>(*result.first->second);
}

#endif /* ALLOY_ENGINE_IMAGE_IMAGE_MANAGER_HPP */
