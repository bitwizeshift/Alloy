#include <alloy/core/traits/function_traits.hpp>

#include <type_traits>

namespace alloy::core::test{

  // Intentionally undefined (used for trait tests)
  // Note: These are **not** declared in an anonymous namespace
  //       since compilers warn on it not being used or implemented.

  void throwing_0_args();
  int throwing_2_args(float, double);
  void nothrow_0_args() noexcept;
  int nothrow_2_args(float, double) noexcept;

  struct example {
    void throwing_0_args();
    int throwing_2_args(float, double);
    void nothrow_0_args() noexcept;
    int nothrow_2_args(float, double) noexcept;

    void const_throwing_0_args();
    int const_throwing_2_args(float, double);
    void const_nothrow_0_args() noexcept;
    int const_nothrow_2_args(float, double) noexcept;

    void lref_throwing_0_args() &;
    void lref_nothrow_0_args() & noexcept;
    void rref_throwing_0_args() &&;
    void rref_nothrow_0_args() && noexcept;
    void const_lref_throwing_0_args() const &;
    void const_lref_nothrow_0_args() const & noexcept;
    void const_rref_throwing_0_args() const &&;
    void const_rref_nothrow_0_args() const && noexcept;
  };

  // ::throwing_0_args
  static_assert(function_traits<&throwing_0_args>::arity == 0);
  static_assert(function_traits<&throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&throwing_0_args>::signature_type,void()>);

  // ::throwing_2_args
  static_assert(function_traits<&throwing_2_args>::arity == 2);
  static_assert(function_traits<&throwing_2_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&throwing_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&throwing_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&throwing_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&throwing_2_args>::signature_type,int(float, double)>);

  // ::nothrow_0_args
  static_assert(function_traits<&nothrow_0_args>::arity == 0);
  static_assert(function_traits<&nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&nothrow_0_args>::signature_type,void() noexcept>);

  // ::nothrow_2_args
  static_assert(function_traits<&nothrow_2_args>::arity == 2);
  static_assert(function_traits<&nothrow_2_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&nothrow_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&nothrow_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&nothrow_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&nothrow_2_args>::signature_type,int(float, double) noexcept>);

  // example
  // example::throwing_0_args
  static_assert(function_traits<&example::throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_0_args>::signature_type,void()>);

  // example::throwing_2_args
  static_assert(function_traits<&example::throwing_2_args>::arity == 2);
  static_assert(function_traits<&example::throwing_2_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&example::throwing_2_args>::signature_type,int(float, double)>);

  // example::nothrow_0_args
  static_assert(function_traits<&example::nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_0_args>::signature_type,void() noexcept>);

  // example::nothrow_2_args
  static_assert(function_traits<&example::nothrow_2_args>::arity == 2);
  static_assert(function_traits<&example::nothrow_2_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&example::nothrow_2_args>::signature_type,int(float, double) noexcept>);

  // example::const_throwing_0_args
  static_assert(function_traits<&example::const_throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::const_throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_0_args>::signature_type,void()>);

  // example::const_throwing_2_args
  static_assert(function_traits<&example::const_throwing_2_args>::arity == 2);
  static_assert(function_traits<&example::const_throwing_2_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&example::const_throwing_2_args>::signature_type,int(float, double)>);

  // example::const_nothrow_0_args
  static_assert(function_traits<&example::const_nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::const_nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_0_args>::signature_type,void() noexcept>);

  // example::const_nothrow_2_args
  static_assert(function_traits<&example::const_nothrow_2_args>::arity == 2);
  static_assert(function_traits<&example::const_nothrow_2_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_2_args>::result_type,int>);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_2_args>::template param_type<0>,float>);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_2_args>::template param_type<1>,double>);
  static_assert(std::is_same_v<typename function_traits<&example::const_nothrow_2_args>::signature_type,int(float, double) noexcept>);

  // example::lref_throwing_0_args
  static_assert(function_traits<&example::lref_throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::lref_throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::lref_throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::lref_throwing_0_args>::signature_type,void()>);

  // example::lref_nothrow_0_args
  static_assert(function_traits<&example::lref_nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::lref_nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::lref_nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::lref_nothrow_0_args>::signature_type,void() noexcept>);

  // example::rref_throwing_0_args
  static_assert(function_traits<&example::rref_throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::rref_throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::rref_throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::rref_throwing_0_args>::signature_type,void()>);

  // example::rref_nothrow_0_args
  static_assert(function_traits<&example::rref_nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::rref_nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::rref_nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::rref_nothrow_0_args>::signature_type,void() noexcept>);

  // example::const_lref_throwing_0_args
  static_assert(function_traits<&example::const_lref_throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::const_lref_throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::const_lref_throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_lref_throwing_0_args>::signature_type,void()>);

  // example::const_lref_nothrow_0_args
  static_assert(function_traits<&example::const_lref_nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::const_lref_nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::const_lref_nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_lref_nothrow_0_args>::signature_type,void() noexcept>);

  // example::rref_throwing_0_args
  static_assert(function_traits<&example::const_rref_throwing_0_args>::arity == 0);
  static_assert(function_traits<&example::const_rref_throwing_0_args>::is_noexcept == false);
  static_assert(std::is_same_v<typename function_traits<&example::const_rref_throwing_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_rref_throwing_0_args>::signature_type,void()>);

  // example::rref_nothrow_0_args
  static_assert(function_traits<&example::const_rref_nothrow_0_args>::arity == 0);
  static_assert(function_traits<&example::const_rref_nothrow_0_args>::is_noexcept == true);
  static_assert(std::is_same_v<typename function_traits<&example::const_rref_nothrow_0_args>::result_type,void>);
  static_assert(std::is_same_v<typename function_traits<&example::const_rref_nothrow_0_args>::signature_type,void() noexcept>);

} // namespace alloy::core::test