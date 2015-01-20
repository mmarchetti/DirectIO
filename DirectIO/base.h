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

#include <Arduino.h>

#if ARDUINO >= 150
// for u8, u16, u32
#include <USBAPI.h>
#else
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
#endif

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
