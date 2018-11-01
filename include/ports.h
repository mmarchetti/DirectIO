/*
  ports.h - Define pin to port/bit mappings for Direct IO library
  Copyright (c) 2015-2018 Michael Marchetti.  All right reserved.

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

#ifndef _PORTS_H
#define _PORTS_H 1

#include "base.h"

#if defined(ARDUINO_ARCH_AVR)
#include "ports_avr.h"
#elif defined(ARDUINO_ARCH_SAM)
#include "ports_sam.h"
#elif defined(ARDUINO_ARCH_SAMD)
#include "ports_samd.h"
#else
#warning "Unsupported Arduino architecture - falling back to digitalRead and digitalWrite."
#define DIRECTIO_FALLBACK 1
#endif

#undef _define_pin
#endif  // _PORTS_H
