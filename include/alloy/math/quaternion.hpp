/*****************************************************************************
 * \file quaternion.hpp
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
#ifndef ALLOY_MATH_QUATERNION_HPP
#define ALLOY_MATH_QUATERNION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"      // core::real
#include "alloy/math/vector/vector3.hpp" // vector3
#include "alloy/math/matrix/matrix3.hpp" // matrix3
#include "alloy/math/matrix/matrix4.hpp" // matrix4
#include "alloy/math/angle/radian.hpp"   // radian
#include "alloy/math/math.hpp"           // almost_equal

#include <cstddef> // std::size_t, std::ptrdiff_t
#include <tuple>   // std::tuple

namespace alloy::math {

  namespace detail {
    struct reproject_axis{};
  }
  /// \brief Function used for tag dispatch
#ifndef ALLOY_DOXYGEN
  inline void reproject_axis( detail::reproject_axis ){}
#else
  auto reproject_axis = implementation defined
#endif

  /// \brief Type used for tag dispatching reprojecting the axis
#ifndef ALLOY_DOXYGEN
  using reproject_axis_t = void(*)( detail::reproject_axis );
#else
  using reproject_axis_t = implementation defined
#endif

  //============================================================================
  // class : quaternion
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A quaternion is a mathematical construct representing
  ///        4-dimensions of a rotation about the x,y,z axis.
  ///
  /// A quaternion is conventionally represented as q = {x,y,z,w}, where
  /// x,y, and z represent rotations around the respective axes
  /// (representing roll, pitch, and yaw respectively).
  //////////////////////////////////////////////////////////////////////////////
  class quaternion
  {
    //--------------------------------------------------------------------------
    // Public member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = core::real;        ///< The underlying value type
    using pointer         = value_type*;       ///< The pointer type
    using reference       = value_type&;       ///< The reference type
    using const_pointer   = const value_type*; ///< The const reference type
    using const_reference = const value_type&; ///< The const pointer type

    using size_type  = std::size_t;           ///< The type used for sizes
    using index_type = std::ptrdiff_t;        ///< The type used for indices

    using vector_type  = vector3<value_type>; ///< The vector type
    using matrix3_type = matrix3<value_type>; ///< The 3x3 matrix type
    using matrix4_type = matrix4<value_type>; ///< The 4x4 matrix type

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a quaternion with an angle of 0
    constexpr quaternion() noexcept;

    /// Constructs a quaternion around an axis
    ///
    /// \param angle the angle of the quaternion
    /// \param axis  the axis of the rotation
    quaternion( radian angle, const vector_type& axis ) noexcept;

    /// \brief Constructs a quaternion from the \p yaw, \p pitch, and
    ///        \p roll angles
    ///
    /// \param yaw the yaw angle
    /// \param pitch the pitch angle
    /// \param roll the roll angle
    quaternion( radian yaw, radian pitch, radian roll ) noexcept;

    /// \brief Constructs a quaternion with only 1 real component
    ///
    /// \param x the real component of the quaternion
    constexpr explicit quaternion( value_type x ) noexcept;

    /// \brief Constructs a quaternion with 4 angle components
    ///
    /// \param x the real component of the quaternion
    /// \param y the first imaginary component of the quaternion
    /// \param z the second imaginary component of the quaternion
    /// \param w the final imaginary component of the quaternion
    constexpr quaternion( value_type x,
                          value_type y,
                          value_type z,
                          value_type w ) noexcept;

    /// Constructs a quaternion with 3 axis vectors
    ///
    /// \param x_axis Vector of the x-axis rotation
    /// \param y_axis Vector of the y-axis rotation
    /// \param z_axis Vector of the z-axis rotation
    quaternion( const vector_type& x_axis,
                const vector_type& y_axis,
                const vector_type& z_axis ) noexcept;

    /// \brief Constructs a quaternion out of a 3-component rotation
    ///        matrix
    ///
    /// \param rot 3x3 rotation matrix
    explicit quaternion( const matrix3_type& rot ) noexcept;

    /// \brief Constructs a quaternion out of a 4-component rotation
    ///        matrix
    ///
    /// \param rot 4x4 rotation matrix
    explicit quaternion( const matrix4_type& rot ) noexcept;

    /// \brief Copy-constructs a quaternion from another quaternion
    ///
    /// \param other the other quaternion to copy
    constexpr quaternion( const quaternion& other ) noexcept = default;

    /// \brief Move-constructs a quaternion from another quaternion
    ///
    /// \param other the other quaternion to move
    constexpr quaternion( quaternion&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the quaternion
    ///
    /// \param other the quaternion to copy
    /// \return reference to \c (*this)
    quaternion& operator=( const quaternion& other ) noexcept = default;

    /// \brief Move-assigns the quaternion
    ///
    /// \param other the quaternion to move
    /// \return reference to \c (*this)
    quaternion& operator=( quaternion&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the w component of this quaternion
    ///
    /// \return reference to the w component
    constexpr reference w() noexcept;
    constexpr const_reference w() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the x component of this quaternion
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the y component of this quaternion
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the z component of this quaternion
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \throw std::out_of_range if indx is out of range
    ///
    /// \param n the element index
    /// \return a reference to the entry
    reference at( index_type n );
    const_reference at( index_type n ) const;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \param n the element index
    /// \return a reference to the entry
    constexpr reference operator[] ( index_type n ) noexcept;
    constexpr const_reference operator[] ( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the x-axis from this quaternion
    ///
    /// \return the x-axis
    vector_type x_axis() const noexcept;

    /// \brief Retrieves the y-axis from this quaternion
    ///
    /// \return the y-axis
    vector_type y_axis() const noexcept;

    /// \brief Retrieves the z-axis from this quaternion
    ///
    /// \return the z-axis
    vector_type z_axis() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Calculates and returns the 3x3 rotation matrix
    ///
    /// \return the rotation matrix
    matrix3_type rotation_matrix() const noexcept;

    /// \brief Calculates and returns the rotation angle and axis
    ///
    /// \return a tuple containing the angle and the axis
    std::tuple<radian,vector_type> angle_axis() const noexcept;

    /// \brief Calculates and returns the x, y, and z axss as vectors
    ///
    /// \return a tuple containing the x, y, and z vector axes
    std::tuple<vector_type,vector_type,vector_type> axes() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Gets the normalized quaternion of \c this
    ///
    /// \param the normalized quaternion of \c this
    quaternion normalized() const noexcept;

    /// \brief Gets the inverse of \c this quaternion
    ///
    /// \return the inverse of \c this quaternion
    quaternion inverse() const noexcept;

    //--------------------------------------------------------------------------
    // Extraction
    //--------------------------------------------------------------------------
  public:

    /// \brief Extracts a 3x3 rotation matrix from this quaternion
    ///
    /// \param rot pointer to the rotation matrix to extract into
    void extract_rotation_matrix( matrix3_type* rot ) const noexcept;

    /// \brief Extracts a 4x4 rotation matrix from this quaternion
    ///
    /// \param rot pointer to the rotation matrix to extract into
    void extract_rotation_matrix( matrix4_type* rot ) const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Extracts the \p angle and \p axis from this quaternion
    ///
    /// \param angle the angle to extract into
    /// \param axis the vector to extract into
    void extract_angle_axis( radian* angle, vector_type* axis ) const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Extracts the x, y, and z axes
    ///
    /// \param x_axis pointer to the x-axis vector
    /// \param y_axis pointer to the y-axis vector
    /// \param z_axis pointer to the z-axis vector
    void extract_axes( vector_type* x_axis,
                       vector_type* y_axis,
                       vector_type* z_axis ) const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this quaternion and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    quaternion& normalize() noexcept;

    /// \brief Inverts this quaternion and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    quaternion& invert() noexcept;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Computes the dot-product of \p rhs and \c this
    ///
    /// \param rhs the quaternion to perform the dot product with
    /// \return the result of the dot product
    core::real dot( const quaternion& rhs ) const noexcept;

    /// \brief Computes the magnitude of this quaternion
    ///
    /// \return the magnitude
    core::real magnitude() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Computes the roll angle by reprojecting the axis
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the roll angle
    radian roll( reproject_axis_t reproject ) const noexcept;

    /// \brief Computes the roll angle by reprojecting the axis
    ///
    /// \return the roll angle
    radian roll() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Computes the pitch angle by reprojecting the axis
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the pitch angle
    radian pitch( reproject_axis_t reproject ) const noexcept;

    /// \brief Computes the pitch angle by reprojecting the axis
    ///
    /// \return the pitch angle
    radian pitch() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Computes the yaw angle by reprojecting the axis
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the yaw angle
    radian yaw( reproject_axis_t reproject ) const noexcept;

    /// \brief Computes the yaw angle by reprojecting the axis
    ///
    /// \return the yaw angle
    radian yaw() const noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    quaternion operator+() const noexcept;
    quaternion operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    quaternion& operator+=( const quaternion& rhs ) noexcept;
    quaternion& operator-=( const quaternion& rhs ) noexcept;
    quaternion& operator*=( const quaternion& rhs ) noexcept;
    quaternion& operator*=( value_type rhs ) noexcept;
    quaternion& operator/=( value_type rhs ) noexcept;
    quaternion& operator/=( const quaternion& rhs ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(16) value_type m_data[4]; ///< The data (w + i*x + j*y + k*z)

    //--------------------------------------------------------------------------
    // Private Member Functions
    //--------------------------------------------------------------------------
  private:

    /// \brief Initializes the quaternion from a \p angle and \p axis
    ///
    /// \param angle the angle
    /// \param axis the axis
    void from_angle_axis( radian angle, const vector_type& axis ) noexcept;

    /// \brief Initializes the quaternion from a \p yaw, \p pitch, and
    ///        \p roll
    ///
    /// \param yaw the yaw angle
    /// \param pitch the pitch angle
    /// \param roll the roll angle
    void from_angles( radian yaw, radian pitch, radian roll ) noexcept;

    /// \brief Initializes the quaternion from a 3x3 rotation matrix
    ///
    /// \param rot the rotation matrix
    void from_rotation_matrix( const matrix3_type& rot ) noexcept;

    /// \brief Initializes the quaternion from a 4x4 rotation matrix
    ///
    /// \param rot the rotation matrix
    void from_rotation_matrix( const matrix4_type& rot ) noexcept;
  };

  //============================================================================
  // non-member functions : class : quaternion
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operations
  //----------------------------------------------------------------------------

  quaternion operator+( const quaternion& lhs,
                        const quaternion& rhs ) noexcept;
  quaternion operator-( const quaternion& lhs,
                        const quaternion& rhs ) noexcept;
  quaternion operator*( const quaternion& lhs,
                        const quaternion& rhs ) noexcept;
  quaternion operator/( const quaternion& lhs,
                        const quaternion& rhs ) noexcept;
  quaternion operator*( const quaternion& lhs,
                        core::real rhs ) noexcept;
  quaternion operator*( core::real lhs,
                        const quaternion& rhs ) noexcept;
  quaternion::vector_type
    operator*( const quaternion& lhs,
               const quaternion::vector_type& rhs ) noexcept;
  quaternion operator/( const quaternion& lhs,
                        quaternion::value_type rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  bool operator==( const quaternion& lhs, const quaternion& rhs ) noexcept;
  bool operator!=( const quaternion& lhs, const quaternion& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether two quaternions are almost equal, relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return \c true if \p lhs almost equals \p rhs
  bool almost_equal( const quaternion& lhs, const quaternion& rhs ) noexcept;

  /// \brief Determines whether two quaternions are almost equal, v to
  ///        \p tolerance
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return \c true if \p lhs almost equals \p rhs
  template<typename Arithmetic, std::enable_if_t<std::is_arithmetic<Arithmetic>::value>* = nullptr>
  bool almost_equal( const quaternion& lhs,
                      const quaternion& rhs,
                      Arithmetic tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return the result of the dot product
  core::real dot( const quaternion& lhs, const quaternion& rhs ) noexcept;

  /// \brief Computes the magnitude of \p x
  ///
  /// \return the magnitude of \p x
  core::real magnitude( const quaternion& x ) noexcept;

  //============================================================================
  // struct : quaternion_constants
  //============================================================================

  struct quaternion_constants final
  {
    ~quaternion_constants() = delete;

    static const quaternion zero;
    static const quaternion identity;
  };

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref quaternion
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_quaternion : std::false_type{};

  template<> struct is_quaternion<quaternion> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_quaternion
  template<typename T>
  constexpr bool is_quaternion_v = is_quaternion<T>::value;

} // namespace alloy::math

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::math::quaternion::quaternion()
  noexcept
  : m_data {
    core::real{1},
    core::real{0},
    core::real{0},
    core::real{0}
  }
{

}

inline alloy::math::quaternion::quaternion( radian angle,
                                            const vector_type& axis )
  noexcept
{
  from_angle_axis(angle,axis);
}

inline alloy::math::quaternion::quaternion( radian yaw,
                                            radian pitch,
                                            radian roll )
  noexcept
{
  from_angles(yaw,pitch,roll);
}

inline constexpr alloy::math::quaternion::quaternion( value_type x )
  noexcept
  : m_data{x,0,0,0}
{

}

inline constexpr alloy::math::quaternion::quaternion( value_type x,
                                                      value_type y,
                                                      value_type z,
                                                      value_type w )
  noexcept
  : m_data{x,y,z,w}
{

}

inline alloy::math::quaternion::quaternion( const vector_type& x_axis,
                                            const vector_type& y_axis,
                                            const vector_type& z_axis )
  noexcept
{
  from_rotation_matrix( matrix3_type{x_axis,y_axis,z_axis} );
}

inline alloy::math::quaternion::quaternion( const matrix3_type& rot )
  noexcept
{
  from_rotation_matrix( rot );
}

inline alloy::math::quaternion::quaternion( const matrix4_type& rot )
  noexcept
{
  from_rotation_matrix( rot );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::math::quaternion::size_type
  alloy::math::quaternion::size()
  const noexcept
{
  return 4;
}

inline constexpr alloy::math::quaternion::reference
  alloy::math::quaternion::w()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::math::quaternion::const_reference
  alloy::math::quaternion::w()
  const noexcept
{
  return m_data[0];
}

inline constexpr alloy::math::quaternion::reference
  alloy::math::quaternion::x()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::math::quaternion::const_reference
  alloy::math::quaternion::x()
  const noexcept
{
  return m_data[1];
}

inline constexpr alloy::math::quaternion::reference
  alloy::math::quaternion::y()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::math::quaternion::const_reference
  alloy::math::quaternion::y()
  const noexcept
{
  return m_data[2];
}

inline constexpr alloy::math::quaternion::reference
  alloy::math::quaternion::z()
  noexcept
{
  return m_data[3];
}

inline constexpr alloy::math::quaternion::const_reference
  alloy::math::quaternion::z()
  const noexcept
{
  return m_data[3];
}

inline constexpr alloy::math::quaternion::pointer
  alloy::math::quaternion::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::math::quaternion::const_pointer
  alloy::math::quaternion::data()
  const noexcept
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::math::quaternion::reference
  alloy::math::quaternion::operator[]( index_type i )
  noexcept
{
  return m_data[i];
}

inline constexpr alloy::math::quaternion::const_reference
  alloy::math::quaternion::operator[]( index_type i )
  const noexcept
{
  return m_data[i];
}

inline alloy::math::quaternion::matrix3_type
  alloy::math::quaternion::rotation_matrix()
  const noexcept
{
  auto mat = matrix3_type{};
  extract_rotation_matrix(&mat);

  return mat;
}

inline std::tuple<alloy::math::radian,alloy::math::quaternion::vector_type>
  alloy::math::quaternion::angle_axis()
  const noexcept
{
  auto rad = radian{};
  auto vec = vector_type{};

  extract_angle_axis(&rad,&vec);
  return std::make_tuple(rad,vec);
}

inline std::tuple<alloy::math::quaternion::vector_type,
                  alloy::math::quaternion::vector_type,
                  alloy::math::quaternion::vector_type>
  alloy::math::quaternion::axes()
  const noexcept
{
  return std::make_tuple( x_axis(), y_axis(), z_axis() );
}

//----------------------------------------------------------------------------

inline alloy::math::quaternion alloy::math::quaternion::normalized()
  const noexcept
{
  return quaternion{*this}.normalize();
}

inline alloy::math::quaternion alloy::math::quaternion::inverse()
  const noexcept
{
  return quaternion{*this}.invert();
}

//----------------------------------------------------------------------------
// Unary Operators
//----------------------------------------------------------------------------

inline alloy::math::quaternion alloy::math::quaternion::operator+()
  const noexcept
{
  return (*this);
}

inline alloy::math::quaternion alloy::math::quaternion::operator-()
  const noexcept
{
  return quaternion( -w(), -x(), -y(), -z() );
}

//==============================================================================
// non-member functions : class : quaternion
//==============================================================================

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline alloy::math::quaternion::value_type
  alloy::math::dot( const quaternion& lhs, const quaternion& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline alloy::math::quaternion::value_type
  alloy::math::magnitude( const quaternion& x )
  noexcept
{
  return x.magnitude();
}

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline alloy::math::quaternion
  alloy::math::operator+( const quaternion& lhs, const quaternion& rhs )
  noexcept
{
  return quaternion{lhs}+=rhs;
}
inline alloy::math::quaternion
  alloy::math::operator-( const quaternion& lhs, const quaternion& rhs )
  noexcept
{
  return quaternion{lhs}-=rhs;
}

inline alloy::math::quaternion
  alloy::math::operator*( const quaternion& lhs, const quaternion& rhs )
  noexcept
{
  return quaternion{lhs}*=rhs;
}

inline alloy::math::quaternion
  alloy::math::operator/( const quaternion& lhs, const quaternion& rhs )
  noexcept
{
  return quaternion{lhs}/=rhs;
}

inline alloy::math::quaternion
  alloy::math::operator*( const quaternion& lhs, quaternion::value_type rhs )
  noexcept
{
  return quaternion{lhs}*=rhs;
}

inline alloy::math::quaternion
  alloy::math::operator*( quaternion::value_type lhs, const quaternion& rhs )
  noexcept
{
  return quaternion{rhs}*=lhs;
}

inline alloy::math::quaternion
  alloy::math::operator/( const quaternion& lhs, quaternion::value_type rhs )
  noexcept
{
  return quaternion{lhs}/=rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline bool alloy::math::operator==( const quaternion& lhs,
                                     const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline bool alloy::math::operator!=( const quaternion& lhs,
                                     const quaternion& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline bool alloy::math::almost_equal( const quaternion& lhs,
                                       const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    if (!almost_equal(lhs[i],rhs[i])) {
      return false;
    }
  }
  return true;
}

template<typename Arithmetic, typename>
inline bool alloy::math::almost_equal( const quaternion& lhs,
                                       const quaternion& rhs,
                                       Arithmetic tolerance )
  noexcept
{
  for (auto i=0; i<4;++i) {
    if (!almost_equal(lhs[i],rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

#endif /* ALLOY_MATH_QUATERNION_HPP */