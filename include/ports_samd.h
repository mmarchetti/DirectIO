/*
  ports_sams.h - SAMD board support for DirectIO and other libraries.
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


#ifndef _PORTS_SAMD_H
#define _PORTS_SAMD_H 1

typedef u32 port_data_t;
typedef volatile port_data_t* port_t;

#define _define_port(NAME, PORTNUM) \
    struct NAME { \
        static const u32 port = u32(&(REG_PORT_DIR##PORTNUM)); \
        static inline u32 port_input_read() { return ((PortGroup*)port)->IN.reg; } \
        static inline void port_output_write(u32 value) { ((PortGroup*)port)->OUT.reg = value; } \
        static inline u32 port_output_read() { return ((PortGroup*)port)->OUT.reg; } \
        static inline void port_enable_outputs(u32 mask) { ((PortGroup*)port)->DIRSET.reg = mask; } \
        static inline void port_enable_inputs(u32 mask) { ((PortGroup*)port)->DIRCLR.reg = mask; } \
    }

#ifdef REG_PORT_DIR0
_define_port(PORT_A, 0);
#endif

#ifdef REG_PORT_DIR1
_define_port(PORT_B, 1);
#endif

#ifdef REG_PORT_DIR2
_define_port(PORT_C, 2);
#endif

#ifdef REG_PORT_DIR3
_define_port(PORT_D, 3);
#endif

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
        static const u32 mask = u32(1) << bit; \
        static inline boolean input_read() { return (PORT::port_input_read() & mask) != 0; } \
        static inline boolean output_write(boolean value) { \
            if(value) { \
               ((PortGroup*)port)->OUTSET.reg = mask; \
            } else { \
                 ((PortGroup*)port)->OUTCLR.reg = mask; \
            } \
        } \
        static inline boolean output_read() { return (PORT::port_output_read() & mask) != 0; } \
    }

#define atomic for(boolean _loop_=(__disable_irq(),true);_loop_; _loop_=(__enable_irq(), false))

#if defined(ARDUINO_SAM_ZERO)
#include "boards/samd/arduino_zero.h"
#elif defined(ARDUINO_SAMD_ZERO)
#include "boards/samd/arduino_mzero.h"
#elif defined(ARDUINO_SAMD_MKR1000)
#include "boards/samd/mkr1000.h"
#elif defined(ARDUINO_SAMD_MKRZERO)
#include "boards/samd/mkrzero.h"
#elif defined(ARDUINO_SAMD_MKRFox1200)
#include "boards/samd/mkrfox1200.h"
#elif defined(ARDUINO_SAMD_MKRGSM1400)
#include "boards/samd/mkrgsm1400.h"
#elif defined(ARDUINO_SAMD_MKRWAN1300)
#include "boards/samd/mkrwan1300.h"
#elif defined(ARDUINO_SAMD_MKRWIFI1010)
#include "boards/samd/mkrwifi1010.h"
#elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
#include "boards/samd/circuitplay.h"
#elif defined(ARDUINO_SAMD_TIAN)
#include "boards/samd/arduino_mzero.h"
#elif defined(ARDUINO_SAMD_ADI)
#include "boards/samd/adi.h"
#elif defined(ARDUINO_SAMD_INDUSTRUINO_D21G)
#include "boards/samd/industruino_d21g.h"
#elif defined(ARDUINO_SAMD_SMARTEVERYTHING_DRAGONFLY)
#include "boards/samd/Dragonfly.h"
#elif defined(ARDUINO_SAMD_SMARTEVERYTHING_LION)
#include "boards/samd/Lion.h"
#elif defined(ARDUINO_SAMD_SMARTEVERYTHING_FOX)
#if defined(ASME3_REVISION)
#include "boards/samd/Fox3.h"
#else
#include "boards/samd/Fox.h"
#endif
#else
#warning "Unsupported Arduino SAMD variant - falling back to digitalRead and digitalWrite."
#define DIRECTIO_FALLBACK 1
#endif

#endif  // _PORTS_SAM_H
