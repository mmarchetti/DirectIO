/*
  ports_avr.h -AVR board support for DirectIO and other libraries.

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

#ifndef _PORTS_AVR_H
#define _PORTS_AVR_H 1

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

#include "base.h"
#undef _SFR_ASM_COMPAT
#undef _AVR_EEPROM_H_
#endif // _AVR_EEPROM_H_

// Define port description constants for all the ports defined in the AVR header file for this CPU.
// See the header files in hardware/tools/avr/avr/avr/ for details.
// Note, this relies on the fact that those headers use #define to define these constants.

typedef u8 port_data_t;
typedef volatile port_data_t* port_t;

// The constants for each port will be defined in a unique struct.
#define _define_port(NAME, IN_REG, OUT_REG, DIR_REG) \
    struct NAME { \
        static const u16 in = IN_REG; \
        static const u16 out = OUT_REG; \
        static const u16 dir = DIR_REG; \
        static inline u8 port_input_read() { return *port_t(in); } \
        static inline void port_output_write(u8 value) { *port_t(out) = value; } \
        static inline u8 port_output_read() { return *port_t(in); } \
        static inline void port_enable_outputs(u8 mask) { *port_t(dir) |= mask; } \
        static inline void port_enable_inputs(u8 mask) { *port_t(dir) &= ~mask; } \
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
        static inline boolean input_read() { return bitRead(*port_t(in), bit); } \
        static inline void output_write(boolean value) { bitWrite(*port_t(out), bit, value); } \
        static inline boolean output_read() { return bitRead(*port_t(in), bit); } \
    }

// Define the correct ports/pins based on the Arduino board selected.
// Arduino IDE 1.5 defines these automatically; if you are using 1.0, you
// must define the correct symbol, e.g.:
// #define ARDUINO_AVR_UNO 1
// #include <DirectIO.h>

#if defined(ARDUINO_AVR_UNO) || \
    defined(ARDUINO_AVR_DUEMILANOVE) || \
    defined(ARDUINO_AVR_NANO) || \
    defined(ARDUINO_AVR_MINI) || \
    defined(ARDUINO_AVR_FIO) || \
    defined(ARDUINO_AVR_BT) || \
    defined(ARDUINO_AVR_LILYPAD) || \
    defined(ARDUINO_AVR_PRO) || \
    defined(ARDUINO_AVR_NG) || \
    defined(ARDUINO_AVR_UNO_WIFI_DEV_ED) || \
    defined(ARDUINO_AVR_FEATHER328P) || \
    defined(ARDUINO_AVR_METRO) || \
    defined(ARDUINO_AVR_PROTRINKET3) || \
    defined(ARDUINO_AVR_PROTRINKET5) || \
    defined(ARDUINO_AVR_PROTRINKET3FTDI) || \
    defined(ARDUINO_AVR_PROTRINKET5FTDI) || \
    defined(ARDUINO_REDBOT)
#include "boards/avr/standard.h"

#elif defined(ARDUINO_AVR_MEGA2560) || \
      defined(ARDUINO_AVR_MEGA) || \
      defined(ARDUINO_AVR_ADK)
#include "boards/avr/mega.h"

#elif defined(ARDUINO_AVR_LEONARDO) || \
      defined(ARDUINO_AVR_LEONARDO_ETH) || \
      defined(ARDUINO_AVR_ESPLORA) || \
      defined(ARDUINO_AVR_LILYPAD_USB)
#include "boards/avr/leonardo.h"

#elif defined(ARDUINO_AVR_MICRO)
#include "boards/avr/micro.h"

#elif defined(ARDUINO_AVR_ROBOT_CONTROL)
#include "boards/avr/robot_control.h"

#elif defined(ARDUINO_AVR_ROBOT_MOTOR)
#include "boards/avr/robot_motor.h"

#elif defined(ARDUINO_AVR_ETHERNET)
#include "boards/avr/ethernet.h"

#elif defined(ARDUINO_AVR_GEMMA)
#include "boards/avr/gemma.h"

#elif defined(ARDUINO_AVR_YUN) || \
      defined(ARDUINO_AVR_YUNMINI) || \
      defined(ARDUINO_AVR_INDUSTRIAL101) || \
      defined(ARDUINO_AVR_LININO_ONE)
#include "boards/avr/yun.h"

#elif defined(ARDUINO_AVR_CIRCUITPLAY)
#include "boards/avr/circuitplay32u4.h"

#elif defined(ARDUINO_AVR_ATMEL_ATMEGA328P_XMINI)
#include "boards/avr/atmega328p-xmini.h"

#elif defined(ARDUINO_AVR_ATMEL_ATMEGA328PB_XMINI)
#include "boards/avr/atmega328pb-xmini.h"

#elif defined(ARDUINO_AVR_ATMEL_ATMEGA168PB_XMINI)
#include "boards/avr/atmega168pb-xmini.h"

#elif defined(ARDUINO_AVR_EMORO_2560)
#include "boards/avr/emoro_variants.h"

#elif defined(ARDUINO_AVR_FLORA8)
#include "boards/avr/flora.h"

#elif defined(ARDUINO_AVR_FEATHER32U4)
#include "boards/avr/feather32u4.h"

#elif defined(ARDUINO_AVR_TRINKET3) || \
      defined(ARDUINO_AVR_TRINKET5) || \
      defined(ARDUINO_AVR_ITSYBITSY32U4_3V) || \
      defined(ARDUINO_AVR_ITSYBITSY32U4_5V)
#include "boards/avr/tiny8.h"

#elif defined(ARDUINO_AVR_BLUEFRUITMICRO)
#include "boards/avr/bluefruitmicro.h"

#elif defined(ARDUINO_AVR_ADAFRUIT32U4)
#include "boards/avr/adafruit32u4.h"

#elif defined(ARDUINO_AVR_OLIMEXINO_328)
#include "boards/avr/Olimexino_328.h"

#elif defined(ARDUINO_AVR_OLIMEXINO_32U4)
#include "boards/avr/Olimexino_32U4.h"

#elif defined(ARDUINO_AVR_OLIMEXINO_Nano)
#include "boards/avr/Olimexino_Nano.h"

#elif defined(ARDUINO_AVR_OLIMEXINO_85)
#include "boards/avr/Olimexino_85.h"

#elif defined(ARDUINO_AVR_RGB_GLASSES)
#include "boards/avr/RGB_Glasses.h"

#elif defined(ARDUINO_AVR_MAKEYMAKEY) || \
      defined(ARDUINO_AVR_PROMICRO) || \
      defined(ARDUINO_AVR_FIOV3) || \
      defined(ARDUINO_AVR_QDUINOMINI)
#include "boards/avr/promicro.h"

#elif defined(ARDUINO_AVR_DIGITAL_SANDBOX)
#include "boards/avr/digitalsandbox.h"

#elif defined(ARDUINO_AVR_SERIAL_7_SEGMENT)
#include "boards/avr/ser7seg.h"

#elif defined(ARDUINO_ATMEGA128RFA1_DEV_BOARD)
#include "boards/avr/rf128.h"

#elif defined(ARDUINO_AVR_LILYPAD_ARDUINO_USB_PLUS_BOARD)
#include "boards/avr/lilypadusbplus.h"

#else
#warning "Unsupported Arduino AVR variant - falling back to digitalRead and digitalWrite. If you are using Arduino IDE 1.0, be sure to #define an Arduino variant (e.g. #define ARDUINO_AVR_UNO 1). See ports.h."
#define DIRECTIO_FALLBACK 1
#endif

#undef _AVR_COMMON_H
#undef _AVR_IO_H_
#undef _AVR_IOXXX_H_
#undef _AVR_SFR_DEFS_H_
#undef _SFR_ASM_COMPAT
#include <avr/io.h>
#include <avr/common.h>

#include <util/atomic.h>
#define atomic ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

#endif  // _PORTS_AVR_H
