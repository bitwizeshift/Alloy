/*****************************************************************************
 * \file select_nth.hpp
 *
 * \brief This header provides a trait for selecting the nth argument of a
 *        variadic set
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
#ifndef ALLOY_CORE_TRAITS_SELECT_NTH_HPP
#define ALLOY_CORE_TRAITS_SELECT_NTH_HPP

#include <cstddef>
#include <type_traits>

template <std::size_t N, typename...Args>
struct select_nth;

template <std::size_t N, typename Arg0, typename...Args>
struct select_nth<N,Arg0,Args...> : select_nth<N-1,Args...>{};

template <std::size_t N>
struct select_nth<N>{}; // empty, to allow for sfinae

template <typename Arg0, typename...Args>
struct select_nth<0,Arg0,Args...>{ using type = Arg0; };

#endif /* ALLOY_CORE_TRAITS_SELECT_NTH_HPP */