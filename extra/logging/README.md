# Extra: `Alloy::Extra::Logging`

**Alloy** bundles the target `Alloy::Extra::Logging` to have a standardized
mechanism for handling logging.

The logger uses C++20 [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format)
for providing a type-safe alternative to `printf`-style formatting, with format
strings checked at compile time.