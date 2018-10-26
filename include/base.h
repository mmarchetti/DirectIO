/*
  base.h - Type definitions for Direct IO and other libraries.
  Copyright (c) 2015 Michael Marchetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _DirectIO_Base_
#define _DirectIO_Base_

#include <Arduino.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef char  i8;
typedef short i16;
typedef long  i32;

// Define std C++ style "<<" operator for writing to output streams.
template<class T> inline Print &operator << (Print& obj, T arg)
{ 
    obj.print(arg); return obj; 
} 

// bits_type(N) gives the smallest type that will hold N bits (0 <= N <= 32)
#define bits_type(N) typename _nbits_t<N>::bits_t

// A little ugly infrastructure to make it work
template <int N> class _nbits_t {};

#define _nbits(N, T) template<> class _nbits_t<N> { public: typedef T bits_t; };

_nbits(0, u8);
_nbits(1, u8);
_nbits(2, u8);
_nbits(3, u8);
_nbits(4, u8);
_nbits(5, u8);
_nbits(6, u8);
_nbits(7, u8);
_nbits(8, u8);
_nbits(9, u16);
_nbits(10, u16);
_nbits(11, u16);
_nbits(12, u16);
_nbits(13, u16);
_nbits(14, u16);
_nbits(15, u16);
_nbits(16, u16);
_nbits(17, u32);
_nbits(18, u32);
_nbits(19, u32);
_nbits(20, u32);
_nbits(21, u32);
_nbits(22, u32);
_nbits(23, u32);
_nbits(24, u32);
_nbits(25, u32);
_nbits(26, u32);
_nbits(27, u32);
_nbits(28, u32);
_nbits(29, u32);
_nbits(30, u32);
_nbits(31, u32);
_nbits(32, u32);

#undef _nbits
#endif // _DirectIO_Base_
