/*****************************************************************************
 * \file macros.hpp
 *
 * \brief
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
#ifndef ALLOY_GRAPHICS_MACROS_HPP
#define ALLOY_GRAPHICS_MACROS_HPP

#ifdef ALLOY_STRINGIZE
# error ALLOY_STRINGIZE defined before macros.hpp included
#endif
#ifdef ALLOY_STRINGIZE_H1
# error ALLOY_STRINGIZE_H1 defined before macros.hpp included
#endif
#ifdef ALLOY_STRINGIZE_H2
# error ALLOY_STRINGIZE_H2 defined before macros.hpp included
#endif

//! \def ALLOY_STRINGIZE(a)
//!
//! \brief Converts arguments to a string representation at compile time.
//!
//! This uses 2 levels of indirection to guarantee macro expansion
#define ALLOY_STRINGIZE(a)           ALLOY_STRINGIZE_H1(a)
#define ALLOY_STRINGIZE_H1(a)        ALLOY_STRINGIZE_H2(a)
#define ALLOY_STRINGIZE_H2(a)        # a

//----------------------------------------------------------------------------

#ifdef ALLOY_JOIN
# error ALLOY_JOIN defined before macros.hpp included
#endif
#ifdef ALLOY_JOIN_H1
# error ALLOY_JOIN_H1 defined before macros.hpp included
#endif
#ifdef ALLOY_JOIN_H2
# error ALLOY_JOIN_H2 defined before macros.hpp included
#endif

//! \def ALLOY_JOIN(lhs,rhs)
//!
//! \brief Concatenates \p lhs to \p rhs at compile time, producing
//!        symbol \a lhsrhs.
//!
//! Concatenates two macro arguments into one. This uses 2 levels of
//! indirection to guarantee macro expansion.
#define ALLOY_JOIN(lhs,rhs)          ALLOY_JOIN_H1(lhs, rhs)
#define ALLOY_JOIN_H1(lhs,rhs)       ALLOY_JOIN_H2(lhs, rhs)
#define ALLOY_JOIN_H2(lhs,rhs)       lhs ## rhs

//----------------------------------------------------------------------------

#ifdef ALLOY_UNIQUE_NAME
# error ALLOY_UNIQUE_NAME defined before macros.hpp included
#endif

//! \def ALLOY_UNIQUE_NAME(name)
//!
//! \brief Constructs an unique name by concatenating \p name with
//!        either a unique counter or the line number (if the counter is
//!        not otherwise available)
#ifdef __COUNTER__
# define ALLOY_UNIQUE_NAME(name) ALLOY_JOIN(name,__COUNTER__)
#else
# define ALLOY_UNIQUE_NAME(name) ALLOY_JOIN(name,__LINE__)
#endif

//----------------------------------------------------------------------------

#ifdef ALLOY_EMPTY
# error ALLOY_EMPTY defined before macros.hpp included
#endif

//! \def ALLOY_EMPTY
//!
//! \brief ANSI-C (C90 standard) does not permit a macro to be invoked with
//!        an empty argument, so this exists to satisfy that requirement.
#define ALLOY_EMPTY /* empty list */

//----------------------------------------------------------------------------

#ifdef ALLOY_ENCODE_WIDE
# error ALLOY_ENCODE_WIDE defined before macros.hpp included
#endif
#ifdef ALLOY_ENCODE_UTF8
# error ALLOY_ENCODE_UTF8 defined before macros.hpp included
#endif
#ifdef ALLOY_ENCODE_UTF16
# error ALLOY_ENCODE_UTF16 defined before macros.hpp included
#endif
#ifdef ALLOY_ENCODE_UTF32
# error ALLOY_ENCODE_UTF32 defined before macros.hpp included
#endif

//! \def ALLOY_ENCODE_WIDE(x)
//!
//! \brief Encodes raw string literal \p x as a wide-string
//!
//! \param x a raw string literal
#define ALLOY_ENCODE_WIDE(x) ALLOY_JOIN(L,x)

//! \def ALLOY_ENCODE_UTF8(x)
//!
//! \brief Encodes raw string literal \p x as a utf8-string
//!
//! \param x a raw string literal
#define ALLOY_ENCODE_UTF8(x) ALLOY_JOIN(u8,x)

//! \def ALLOY_ENCODE_UTF16(x)
//!
//! \brief Encodes raw string literal \p x as a utf16-string
//!
//! \param x a raw string literal
#define ALLOY_ENCODE_UTF16(x) ALLOY_JOIN(u,x)

//! \def ALLOY_ENCODE_UTF32(x)
//!
//! \brief Encodes raw string literal \p x as a utf32-string
//!
//! \param x a raw string literal
#define ALLOY_ENCODE_UTF32(x) ALLOY_JOIN(U,x)

//----------------------------------------------------------------------------
// Boolean Operations
//----------------------------------------------------------------------------

#ifdef ALLOY_TO_BOOL
# error ALLOY_TO_BOOL defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_true
# error ALLOY_TO_BOOL_true defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_false
# error ALLOY_TO_BOOL_false defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_0
# error ALLOY_TO_BOOL_0 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_1
# error ALLOY_TO_BOOL_1 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_2
# error ALLOY_TO_BOOL_2 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_3
# error ALLOY_TO_BOOL_3 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_4
# error ALLOY_TO_BOOL_4 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_5
# error ALLOY_TO_BOOL_5 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_6
# error ALLOY_TO_BOOL_6 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_7
# error ALLOY_TO_BOOL_7 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_8
# error ALLOY_TO_BOOL_8 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_9
# error ALLOY_TO_BOOL_9 defined before macros.hpp included
#endif
#ifdef ALLOY_TO_BOOL_10
# error ALLOY_TO_BOOL_10 defined before macros.hpp included
#endif

//! \def ALLOY_TO_BOOL(x)
//!
//! \brief Expands the integer \p x into '0' if the result is falsey,
//!        '1' otherwise
//!
//! This is necessary to do macro conditionals
//!
//! \param x the x argument
#define ALLOY_TO_BOOL(x) ALLOY_JOIN(ALLOY_TO_BOOL_, x)
#define ALLOY_TO_BOOL_true  1 // true and false shouldn't be used in macros
#define ALLOY_TO_BOOL_false 0 // but are added here anyway just in case
#define ALLOY_TO_BOOL_0     0
#define ALLOY_TO_BOOL_1     1
#define ALLOY_TO_BOOL_2     1
#define ALLOY_TO_BOOL_3     1
#define ALLOY_TO_BOOL_4     1
#define ALLOY_TO_BOOL_5     1
#define ALLOY_TO_BOOL_6     1
#define ALLOY_TO_BOOL_7     1
#define ALLOY_TO_BOOL_8     1
#define ALLOY_TO_BOOL_9     1
#define ALLOY_TO_BOOL_10    1
// Hopefully 10 is enough to macro preprocessing. This can be extended if
// this proves too short

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_NOT
# error ALLOY_BOOL_NOT defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_NOT_0
# error ALLOY_BOOL_NOT_0 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_NOT_1
# error ALLOY_BOOL_NOT_1 defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_NOT(x)
//!
//! \brief Expands into '1' if \p x is '0', else '0'
//!
//! \param x the x argument
#define ALLOY_BOOL_NOT(x)  ALLOY_JOIN(ALLOY_BOOL_NOT_, ALLOY_TO_BOOL(x))
#define ALLOY_BOOL_NOT_0   1
#define ALLOY_BOOL_NOT_1   0

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_OR
# error ALLOY_BOOL_OR defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_OR_00
# error ALLOY_BOOL_OR_00 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_OR_10
# error ALLOY_BOOL_OR_10 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_OR_11
# error ALLOY_BOOL_OR_11 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_OR_01
# error ALLOY_BOOL_OR_01 defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_OR(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise OR. The result of (x||y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_OR(x,y) ALLOY_JOIN(ALLOY_JOIN(ALLOY_BOOL_OR_, ALLOY_TO_BOOL(x)),ALLOY_TO_BOOL(y))
#define ALLOY_BOOL_OR_00   0
#define ALLOY_BOOL_OR_10   1
#define ALLOY_BOOL_OR_11   1
#define ALLOY_BOOL_OR_01   1

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_AND
# error ALLOY_BOOL_AND defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_AND_00
# error ALLOY_BOOL_AND_00 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_AND_10
# error ALLOY_BOOL_AND_10 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_AND_11
# error ALLOY_BOOL_AND_11 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_AND_01
# error ALLOY_BOOL_AND_01 defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_AND(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise AND. The result of (x&&y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_AND(x,y)  ALLOY_JOIN(ALLOY_JOIN(ALLOY_BOOL_AND_, ALLOY_TO_BOOL(x)),ALLOY_TO_BOOL(y))
#define ALLOY_BOOL_AND_00  0
#define ALLOY_BOOL_AND_10  0
#define ALLOY_BOOL_AND_11  1
#define ALLOY_BOOL_AND_01  0

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_XOR
# error ALLOY_BOOL_XOR defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_XOR_00
# error ALLOY_BOOL_XOR_00 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_XOR_10
# error ALLOY_BOOL_XOR_10 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_XOR_11
# error ALLOY_BOOL_XOR_11 defined before macros.hpp included
#endif
#ifdef ALLOY_BOOL_XOR_01
# error ALLOY_BOOL_XOR_01 defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_XOR(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise XOR. The result of (x^y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_XOR(x,y)  ALLOY_JOIN(ALLOY_JOIN(ALLOY_BOOL_XOR_, ALLOY_TO_BOOL(x)),ALLOY_TO_BOOL(y))
#define ALLOY_BOOL_XOR_00  0
#define ALLOY_BOOL_XOR_10  1
#define ALLOY_BOOL_XOR_01  1
#define ALLOY_BOOL_XOR_11  0

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_NAND
# error ALLOY_BOOL_NAND defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_NAND(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise AND. The result of !(x&&y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_NAND(x,y) ALLOY_BOOL_NOT(ALLOY_BOOL_AND(x,y))

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_NOR
# error ALLOY_BOOL_NOR defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_NOR(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise NOR. The result of !(x||y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_NOR(x,y)  ALLOY_BOOL_NOT(ALLOY_BOOL_OR(x,y))

//----------------------------------------------------------------------------

#ifdef ALLOY_BOOL_XNOR
# error ALLOY_BOOL_XNOR defined before macros.hpp included
#endif

//! \def ALLOY_BOOL_XNOR(x,y)
//!
//! \brief Expands into 1 or 0 by performing bitwise XNOR. The result of !(x^y)
//!
//! \param x the x argument
//! \param y the y argument
#define ALLOY_BOOL_XNOR(x,y) ALLOY_BOOL_NOT(ALLOY_BOOL_XOR(x,y))

//----------------------------------------------------------------------------
// Conditionals
//----------------------------------------------------------------------------

#ifdef ALLOY_IF
# error ALLOY_IF defined before macros.hpp included
#endif
#ifdef ALLOY_IF_0
# error ALLOY_IF_0 defined before macros.hpp included
#endif
#ifdef ALLOY_IF_1
# error ALLOY_IF_1 defined before macros.hpp included
#endif

//! \def ALLOY_IF(cond,x)
//!
//! \brief Expands into \p x if \p cond is truthy
//!
//! \param cond the condition to check
//! \param x the true-case to expand
#define ALLOY_IF(cond,x) ALLOY_JOIN(ALLOY_IF_, ALLOY_TO_BOOL(cond))(x)
#define ALLOY_IF_0(x)
#define ALLOY_IF_1(x)    x

//----------------------------------------------------------------------------

#ifdef ALLOY_IF_ELSE
# error ALLOY_IF defined before macros.hpp included
#endif
#ifdef ALLOY_IF_ELSE_0
# error ALLOY_IF_0 defined before macros.hpp included
#endif
#ifdef ALLOY_IF_ELSE_1
# error ALLOY_IF_1 defined before macros.hpp included
#endif

//! \def ALLOY_IF_ELSE(cond,x,y)
//!
//! \brief Expands into \p x if \p cond is truthy, otherwise \p y
//!
//! \param cond the condition to check
//! \param x the true-case to expand
//! \param y the false-case to expand
#define ALLOY_IF_ELSE(cond,x,y) ALLOY_JOIN(ALLOY_IF_ELSE_, ALLOY_TO_BOOL(cond))(x,y)
#define ALLOY_IF_ELSE_0(x,y) y
#define ALLOY_IF_ELSE_1(x,y) x

//----------------------------------------------------------------------------

#ifdef ALLOY_COMMA_IF
# error ALLOY_IF defined before macros.hpp included
#endif
#ifdef ALLOY_COMMA_IF_0
# error ALLOY_IF_0 defined before macros.hpp included
#endif
#ifdef ALLOY_COMMA_IF_1
# error ALLOY_IF_1 defined before macros.hpp included
#endif

//! \def ALLOY_COMMA_IF(cond)
//!
//! \brief Expands into a comma (',') if \p cond is truthy
//!
//! \param cond the condition to check
#define ALLOY_COMMA_IF(cond) ALLOY_JOIN(ALLOY_COMMA_IF_, ALLOY_TO_BOOL(cond))
#define ALLOY_COMMA_IF_0
#define ALLOY_COMMA_IF_1 ,

//----------------------------------------------------------------------------
// List Generation
//----------------------------------------------------------------------------

#ifdef ALLOY_LIST
# error ALLOY_LIST defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_0
# error ALLOY_LIST_0 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_1
# error ALLOY_LIST_1 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_2
# error ALLOY_LIST_2 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_3
# error ALLOY_LIST_3 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_4
# error ALLOY_LIST_4 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_5
# error ALLOY_LIST_5 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_6
# error ALLOY_LIST_6 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_7
# error ALLOY_LIST_7 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_8
# error ALLOY_LIST_8 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_9
# error ALLOY_LIST_9 defined before macros.hpp included
#endif
#ifdef ALLOY_LIST_10
# error ALLOY_LIST_10 defined before macros.hpp included
#endif

//! \def ALLOY_LIST(count,prefix)
//!
//! \brief Expands into \count comma-delimited elements in a sequence,
//!        with each element prefixed by \p prefix and suffixed by its
//!        index.
//!
//! Example Use:
//! \code
//!    ALLOY_LIST(5,var)         => var1, var2, var3, var4, var5
//!    ALLOY_LIST(3,int x)       => int x1, int x2, int x3
//!    ALLOY_LIST(3,ALLOY_EMPTY) => 1, 2, 3
//! \endcode
//!
//! \param count the number of elements to have in the list
//! \param prefix the prefix to put on each element
#define ALLOY_LIST(count,prefix)         ALLOY_JOIN(ALLOY_LIST_,count)(prefix)
#define ALLOY_LIST_0(x)
#define ALLOY_LIST_1(x)  x##1
#define ALLOY_LIST_2(x)  ALLOY_LIST_1(x), x##2
#define ALLOY_LIST_3(x)  ALLOY_LIST_2(x), x##3
#define ALLOY_LIST_4(x)  ALLOY_LIST_3(x), x##4
#define ALLOY_LIST_5(x)  ALLOY_LIST_4(x), x##5
#define ALLOY_LIST_6(x)  ALLOY_LIST_5(x), x##6
#define ALLOY_LIST_7(x)  ALLOY_LIST_6(x), x##7
#define ALLOY_LIST_8(x)  ALLOY_LIST_7(x), x##8
#define ALLOY_LIST_9(x)  ALLOY_LIST_8(x), x##9
#define ALLOY_LIST_10(x) ALLOY_LIST_10(x), x##10

//----------------------------------------------------------------------------

#ifdef ALLOY_REVERSE_LIST
# error ALLOY_REVERSE_LIST defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_0
# error ALLOY_REVERSE_LIST_0 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_1
# error ALLOY_REVERSE_LIST_1 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_2
# error ALLOY_REVERSE_LIST_2 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_3
# error ALLOY_REVERSE_LIST_3 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_4
# error ALLOY_REVERSE_LIST_4 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_5
# error ALLOY_REVERSE_LIST_5 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_6
# error ALLOY_REVERSE_LIST_6 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_7
# error ALLOY_REVERSE_LIST_7 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_8
# error ALLOY_REVERSE_LIST_8 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_9
# error ALLOY_REVERSE_LIST_9 defined before macros.hpp included
#endif
#ifdef ALLOY_REVERSE_LIST_10
# error ALLOY_REVERSE_LIST_10 defined before macros.hpp included
#endif

//! \def ALLOY_REVERSE_LIST(count,prefix)
//!
//! \brief Expands into \count comma-delimited elements in a sequence,
//!        with each element prefixed by \p prefix and suffixed by
//!        its index. The sequence is done in reverse, by decrementing
//!        the index starting with the max count.
//!
//! Example Use:
//! \code
//!    ALLOY_REVERSE_LIST(5,var)         => var5, var4, var3, var2, var1
//!    ALLOY_REVERSE_LIST(3,int x)       => int x3, int x2, int x1
//!    ALLOY_REVERSE_LIST(3,ALLOY_EMPTY) => 3, 2, 1
//! \endcode
//!
//! \param count the number of elements to have in the list
//! \param prefix the prefix to put on each element
#define ALLOY_REVERSE_LIST(count,prefix) ALLOY_JOIN(ALLOY_REVERSE_LIST_,count)(prefix)
#define ALLOY_REVERSE_LIST_0(x)
#define ALLOY_REVERSE_LIST_1(x)  x##1
#define ALLOY_REVERSE_LIST_2(x)  x##2, ALLOY_REVERSE_LIST_1(x)
#define ALLOY_REVERSE_LIST_3(x)  x##3, ALLOY_REVERSE_LIST_2(x)
#define ALLOY_REVERSE_LIST_4(x)  x##4, ALLOY_REVERSE_LIST_3(x)
#define ALLOY_REVERSE_LIST_5(x)  x##5, ALLOY_REVERSE_LIST_4(x)
#define ALLOY_REVERSE_LIST_6(x)  x##6, ALLOY_REVERSE_LIST_5(x)
#define ALLOY_REVERSE_LIST_7(x)  x##7, ALLOY_REVERSE_LIST_6(x)
#define ALLOY_REVERSE_LIST_8(x)  x##8, ALLOY_REVERSE_LIST_7(x)
#define ALLOY_REVERSE_LIST_9(x)  x##9, ALLOY_REVERSE_LIST_8(x)
#define ALLOY_REVERSE_LIST_10(x) x##10, ALLOY_REVERSE_LIST_9(x)

//----------------------------------------------------------------------------

#ifdef ALLOY_SELECT
# error ALLOY_SELECT defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_0
# error ALLOY_SELECT_0 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_1
# error ALLOY_SELECT_1 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_2
# error ALLOY_SELECT_2 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_3
# error ALLOY_SELECT_3 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_4
# error ALLOY_SELECT_4 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_5
# error ALLOY_SELECT_5 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_6
# error ALLOY_SELECT_6 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_7
# error ALLOY_SELECT_7 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_8
# error ALLOY_SELECT_8 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_9
# error ALLOY_SELECT_9 defined before macros.hpp included
#endif
#ifdef ALLOY_SELECT_10
# error ALLOY_SELECT_10 defined before macros.hpp included
#endif

//! \def ALLOY_SELECT(n,...)
//!
//! \brief Expands to the nth argument in the variadic arguments
//!
//! \param n the nth element to select
//! \param ... the arguments to select from
#define ALLOY_SELECT(n,...) ALLOY_JOIN(ALLOY_SELECT_,n)(__VA_ARGS__, \
                                                        a1, \
                                                        a2, \
                                                        a3, \
                                                        a4, \
                                                        a5, \
                                                        a6, \
                                                        a7, \
                                                        a8, \
                                                        a9, \
                                                        a10)
#define ALLOY_SELECT_1(a1, ...) a1
#define ALLOY_SELECT_2(a1, a2, ...) a2
#define ALLOY_SELECT_3(a1, a2, a3, ...) a3
#define ALLOY_SELECT_4(a1, a2, a3, a4, ...) a4
#define ALLOY_SELECT_5(a1, a2, a3, a4, a5, ...) a5
#define ALLOY_SELECT_6(a1, a2, a3, a4, a5, a6, ...) a6
#define ALLOY_SELECT_7(a1, a2, a3, a4, a5, a6, a7, ...) a7
#define ALLOY_SELECT_8(a1, a2, a3, a4, a5, a6, a7, a8, ...) a8
#define ALLOY_SELECT_9(a1, a2, a3, a4, a5, a6, a7, a8, a9, ...) a9
#define ALLOY_SELECT_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10

//----------------------------------------------------------------------------

#ifdef ALLOY_COUNT_VA_ARGS
# error ALLOY_COUNT_VA_ARGS defined before macros.hpp included
#endif
#ifdef ALLOY_COUNT_VA_ARGS_H
# error ALLOY_COUNT_VA_ARGS_H defined before macros.hpp included
#endif

//! \def ALLOY_COUNT_VA_ARGS(...)
//!
//! \brief Expands into the number of arguments passed to this macro
//!
//! \param ... the variadic arguments passed to this macro
#define ALLOY_COUNT_VA_ARGS(...) \
  ALLOY_COUNT_VA_ARGS_H(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ALLOY_COUNT_VA_ARGS_H(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) \
  N

//----------------------------------------------------------------------------

#ifdef ALLOY_VA_FIRST
# error ALLOY_VA_FIRST defined before macros.hpp included
#endif
#ifdef ALLOY_VA_FIRST_H
# error ALLOY_VA_FIRST_H defined before macros.hpp included
#endif

//! \def ALLOY_VA_FIRST(...)
//!
//! \brief Expands into the first argument of the variadic arguments
//!
//! \param ... the variadic arguments passed to this macro
#define ALLOY_VA_FIRST(...) ALLOY_VA_FIRST_H(__VA_ARGS__,throwaway)
#define ALLOY_VA_FIRST_H(first,...) first

//----------------------------------------------------------------------------

#ifdef ALLOY_VA_REST
# error ALLOY_VA_REST defined before macros.hpp included
#endif
#ifdef ALLOY_VA_REST_H
# error ALLOY_VA_REST_H defined before macros.hpp included
#endif
#ifdef ALLOY_VA_REST_H1
# error ALLOY_VA_REST_H1 defined before macros.hpp included
#endif
#ifdef ALLOY_VA_REST_H0
# error ALLOY_VA_REST_H0 defined before macros.hpp included
#endif

//! \def ALLOY_VA_REST(...)
//!
//! \brief Expands into the remaining variadic arguments after the first one
//!
//! \param ... the variadic arguments
#define ALLOY_VA_REST(...) ALLOY_VA_REST_H(__VA_ARGS__)
#define ALLOY_VA_REST_H(first, ...) \
  ALLOY_JOIN(ALLOY_VA_REST_H,ALLOY_TO_BOOL(ALLOY_COUNT_VA_ARGS(__VA_ARGS__)))
#define ALLOY_VA_REST_H0(first) // no remaining args
#define ALLOY_VA_REST_H1(first,...) __VA_ARGS__

#endif /* ALLOY_GRAPHICS_MACROS_HPP */