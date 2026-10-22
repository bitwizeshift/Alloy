// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file select_nth.hpp
 *
 * \brief This header provides a trait for selecting the nth argument of a
 *        variadic set
 *****************************************************************************/

#ifndef ALLOY_CORE_TRAITS_SELECT_NTH_HPP
#define ALLOY_CORE_TRAITS_SELECT_NTH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef>

template <std::size_t N, typename...Args>
struct select_nth;

template <std::size_t N, typename Arg0, typename...Args>
struct select_nth<N,Arg0,Args...> : select_nth<N-1,Args...>{};

template <std::size_t N>
struct select_nth<N>{}; // empty, to allow for sfinae

template <typename Arg0, typename...Args>
struct select_nth<0,Arg0,Args...>{ using type = Arg0; };

#endif /* ALLOY_CORE_TRAITS_SELECT_NTH_HPP */
