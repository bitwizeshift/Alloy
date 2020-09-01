#include "alloy/core/utilities/string_tokenizer.hpp"

#include <string>

namespace alloy::core {
namespace {

  class tokenizer_error_category : public std::error_category
  {
  public:

    auto name() const noexcept -> const char* override {
      return "string tokenizer";
    }

    auto message(int ec) const -> std::string override {
      switch (ec) {
        case 0: {
          return "no error";
        }
        case static_cast<int>(tokenizer_error::out_of_tokens): {
          return "out of tokens";
        }
        default: {
          return "unknown error code " + std::to_string(ec);
        }
      }
    }
  };

} // namespace <anonymous>
} // namespace alloy::core

auto alloy::core::make_error_code(tokenizer_error error)
  noexcept -> std::error_code
{
  static const auto s_category = tokenizer_error_category{};

  return std::error_code{
    static_cast<int>(error),
    s_category,
  };
}
