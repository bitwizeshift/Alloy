#ifndef ALLOY_MATH_TYPES_HPP
#define ALLOY_MATH_TYPES_HPP

namespace alloy {

  /// \brief The value which represents the floating point precision
#ifndef ALLOY_MATH_DOUBLE_PRECISION
  using real = double;
#else
  using real = float;
#endif

} // namespace alloy

#endif /* ALLOY_MATH_TYPES */