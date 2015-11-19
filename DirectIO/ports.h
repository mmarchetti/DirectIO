/*
  ports.h - Define pin to port/bit mappings for Direct IO library
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

#ifndef _PORTS_H
#define _PORTS_H 1

#undef _AVR_COMMON_H
#undef _AVR_IO_H_
#undef _AVR_IOXXX_H_
#undef _AVR_SFR_DEFS_H_
#undef _SFR_ASM_COMPAT

// _SFR_ASM_COMPAT enables the port numbers (e.g. PORTA) to be plain integers so they can be used as template parameters.
#define _SFR_ASM_COMPAT 1
#include <avr/io.h>

#ifndef _AVR_EEPROM_H_
// avr/eeprom.h isn't compatible with _SFR_ASM_COMPAT, so prevent its inclusion
#define _AVR_EEPROM_H_ 1

#include <base.h>
#undef _SFR_ASM_COMPAT
#undef _AVR_EEPROM_H_
#endif // _AVR_EEPROM_H_

// Define port description constants for all the ports defined in the AVR header file for this CPU.
// See the header files in hardware/tools/avr/avr/include/avr/ for details.
// Note, this relies on the fact that those headers use #define to define these constants.

// The constants for each port will be defined in a unique struct.
#define _define_port(NAME, IN_REG, OUT_REG, DIR_REG) \
    struct NAME { \
        static const u16 in = IN_REG; \
        static const u16 out = OUT_REG; \
        static const u16 dir = DIR_REG; \
    }

#ifdef PINA
_define_port(PORT_A, PINA, PORTA, DDRA);
#endif

#ifdef PINB
_define_port(PORT_B, PINB, PORTB, DDRB);
#endif

#ifdef PINC
_define_port(PORT_C, PINC, PORTC, DDRC);
#endif

#ifdef PIND
_define_port(PORT_D, PIND, PORTD, DDRD);
#endif

#ifdef PINE
_define_port(PORT_E, PINE, PORTE, DDRE);
#endif

#ifdef PINF
_define_port(PORT_F, PINF, PORTF, DDRF);
#endif

#ifdef PING
_define_port(PORT_G, PING, PORTG, DDRG);
#endif

#ifdef PINH
_define_port(PORT_H, PINH, PORTH, DDRH);
#endif

#ifdef PINI
// don't think this exists even on Mega 2560, but defining for completeness
_define_port(PORT_I, PINI, PORTI, DDRI);
#endif

#ifdef PINJ
_define_port(PORT_J, PINJ, PORTJ, DDRJ);
#endif

#ifdef PINK
_define_port(PORT_K, PINK, PORTK, DDRK);
#endif

#ifdef PINL
_define_port(PORT_L, PINL, PORTL, DDRL);
#endif

#undef _define_port

// These constants are propagated in a template-friendly way by using (what else? :) templates.
// First, we define a generic template for a class describing the pin data.
// Since this doesn't describe a specific pin, there is no data present.
template <u8 pin> struct _pins {};

// Next we will create a template specialization for each defined pin number.
// Each specialization will contain the constants for that pin.
// To avoid a lot of repetitive code, we will define the specializations with a macro.
// Also note that each pin inherits the port registers from the
// corresponding port object (defined above).

#define _define_pin(PIN, PORT, BIT) \
    template <> struct _pins<PIN> : public PORT { \
        static const u8 bit = BIT; \
    }

// Define the correct ports/pins based on the Arduino board selected.
// Arduino IDE 1.5 defines these automatically; if you are using 1.0, you
// must define the correct symbol, e.g.:
// #define ARDUINO_AVR_UNO 1
// #include <direct_io.h>
// The definitions below match what's defined in pins_arduino.h for each config.

#if defined(ARDUINO_AVR_UNO) || \
	defined(ARDUINO_AVR_YUN) || \
	defined(ARDUINO_AVR_DUEMILANOVE) || \
	defined(ARDUINO_AVR_NANO) || \
	defined(ARDUINO_AVR_MINI) || \
	defined(ARDUINO_AVR_ETHERNET) || \
	defined(ARDUINO_AVR_FIO) || \
	defined(ARDUINO_AVR_BT) || \
	defined(ARDUINO_AVR_LILYPAD) || \
	defined(ARDUINO_AVR_PRO) || \
	defined(ARDUINO_AVR_NG)

_define_pin(0, PORT_D, 0);
_define_pin(1, PORT_D, 1);
_define_pin(2, PORT_D, 2);
_define_pin(3, PORT_D, 3);
_define_pin(4, PORT_D, 4);
_define_pin(5, PORT_D, 5);
_define_pin(6, PORT_D, 6);
_define_pin(7, PORT_D, 7);
_define_pin(8, PORT_B, 0);
_define_pin(9, PORT_B, 1);
_define_pin(10, PORT_B, 2);
_define_pin(11, PORT_B, 3);
_define_pin(12, PORT_B, 4);
_define_pin(13, PORT_B, 5);
_define_pin(14, PORT_C, 0);
_define_pin(15, PORT_C, 1);
_define_pin(16, PORT_C, 2);
_define_pin(17, PORT_C, 3);
_define_pin(18, PORT_C, 4);
_define_pin(19, PORT_C, 5);

#elif defined(ARDUINO_AVR_MEGA2560) || \
	defined(ARDUINO_AVR_ADK)

_define_pin(0, PORT_E, 0);
_define_pin(1, PORT_E, 1);
_define_pin(2, PORT_E, 4);
_define_pin(3, PORT_E, 5);
_define_pin(4, PORT_G, 5);
_define_pin(5, PORT_E, 3);
_define_pin(6, PORT_H, 3);
_define_pin(7, PORT_H, 4);
_define_pin(8, PORT_H, 5);
_define_pin(9, PORT_H, 6);
_define_pin(10, PORT_B, 4);
_define_pin(11, PORT_B, 5);
_define_pin(12, PORT_B, 6);
_define_pin(13, PORT_B, 7);
_define_pin(14, PORT_J, 1);
_define_pin(15, PORT_J, 0);
_define_pin(16, PORT_H, 1);
_define_pin(17, PORT_H, 0);
_define_pin(18, PORT_D, 3);
_define_pin(19, PORT_D, 2);
_define_pin(20, PORT_D, 1);
_define_pin(21, PORT_D, 0);
_define_pin(22, PORT_A, 0);
_define_pin(23, PORT_A, 1);
_define_pin(24, PORT_A, 2);
_define_pin(25, PORT_A, 3);
_define_pin(26, PORT_A, 4);
_define_pin(27, PORT_A, 5);
_define_pin(28, PORT_A, 6);
_define_pin(29, PORT_A, 7);
_define_pin(30, PORT_C, 7);
_define_pin(31, PORT_C, 6);
_define_pin(32, PORT_C, 5);
_define_pin(33, PORT_C, 4);
_define_pin(34, PORT_C, 3);
_define_pin(35, PORT_C, 2);
_define_pin(36, PORT_C, 1);
_define_pin(37, PORT_C, 0);
_define_pin(38, PORT_D, 7);
_define_pin(39, PORT_G, 2);
_define_pin(40, PORT_G, 1);
_define_pin(41, PORT_G, 0);
_define_pin(42, PORT_L, 7);
_define_pin(43, PORT_L, 6);
_define_pin(44, PORT_L, 5);
_define_pin(45, PORT_L, 4);
_define_pin(46, PORT_L, 3);
_define_pin(47, PORT_L, 2);
_define_pin(48, PORT_L, 1);
_define_pin(49, PORT_L, 0);
_define_pin(50, PORT_B, 3);
_define_pin(51, PORT_B, 2);
_define_pin(52, PORT_B, 1);
_define_pin(53, PORT_B, 0);
_define_pin(54, PORT_F, 0);
_define_pin(55, PORT_F, 1);
_define_pin(56, PORT_F, 2);
_define_pin(57, PORT_F, 3);
_define_pin(58, PORT_F, 4);
_define_pin(59, PORT_F, 5);
_define_pin(60, PORT_F, 6);
_define_pin(61, PORT_F, 7);
_define_pin(62, PORT_K, 0);
_define_pin(63, PORT_K, 1);
_define_pin(64, PORT_K, 2);
_define_pin(65, PORT_K, 3);
_define_pin(66, PORT_K, 4);
_define_pin(67, PORT_K, 5);
_define_pin(68, PORT_K, 6);
_define_pin(69, PORT_K, 7);

#elif defined(ARDUINO_AVR_LEONARDO) || \
	defined(ARDUINO_AVR_MICRO) || \
	defined(ARDUINO_AVR_ESPLORA) || \
	defined(ARDUINO_AVR_LILYPAD_USB) || \
	defined(ARDUINO_AVR_ROBOT_MOTOR) || \
	defined(ARDUINO_AVR_ROBOT_CONTROL)

_define_pin(0, PORT_D, 2);
_define_pin(1, PORT_D, 3);
_define_pin(2, PORT_D, 1);
_define_pin(3, PORT_D, 0);
_define_pin(4, PORT_D, 4);
_define_pin(5, PORT_C, 6);
_define_pin(6, PORT_D, 7);
_define_pin(7, PORT_E, 6);
_define_pin(8, PORT_B, 4);
_define_pin(9, PORT_B, 5);
_define_pin(10, PORT_B, 6);
_define_pin(11, PORT_B, 7);
_define_pin(12, PORT_D, 6);
_define_pin(13, PORT_C, 7);
_define_pin(14, PORT_B, 3);
_define_pin(15, PORT_B, 1);
_define_pin(16, PORT_B, 2);
_define_pin(17, PORT_B, 0);
_define_pin(18, PORT_F, 7);
_define_pin(19, PORT_F, 6);
_define_pin(20, PORT_F, 5);
_define_pin(21, PORT_F, 4);
_define_pin(22, PORT_F, 1);
_define_pin(23, PORT_F, 0);
_define_pin(24, PORT_D, 4);
_define_pin(25, PORT_D, 7);
_define_pin(26, PORT_B, 4);
_define_pin(27, PORT_B, 5);
_define_pin(28, PORT_B, 6);
_define_pin(29, PORT_D, 6);

#else

#warning "Unsupported Arduino variant - assuming a Standard variant. If you are using Arduino IDE 1.0, be sure to #define an Arduino variant (e.g. #define ARDUINO_AVR_UNO 1). See ports.h."

// same as standard variant above
_define_pin(0, PORT_D, 0);
_define_pin(1, PORT_D, 1);
_define_pin(2, PORT_D, 2);
_define_pin(3, PORT_D, 3);
_define_pin(4, PORT_D, 4);
_define_pin(5, PORT_D, 5);
_define_pin(6, PORT_D, 6);
_define_pin(7, PORT_D, 7);
_define_pin(8, PORT_B, 0);
_define_pin(9, PORT_B, 1);
_define_pin(10, PORT_B, 2);
_define_pin(11, PORT_B, 3);
_define_pin(12, PORT_B, 4);
_define_pin(13, PORT_B, 5);
_define_pin(14, PORT_C, 0);
_define_pin(15, PORT_C, 1);
_define_pin(16, PORT_C, 2);
_define_pin(17, PORT_C, 3);
_define_pin(18, PORT_C, 4);
_define_pin(19, PORT_C, 5);

#endif

#undef _define_pin

#undef _AVR_COMMON_H
#undef _AVR_IO_H_
#undef _AVR_IOXXX_H_
#undef _AVR_SFR_DEFS_H_
#undef _SFR_ASM_COMPAT
#include <avr/io.h>
#include <avr/common.h>

#include <util/atomic.h>
#define atomic ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

#endif  // _PORTS_H
