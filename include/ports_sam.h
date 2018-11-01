/*
  ports_sam.h - SAM board support for DirectIO and other libraries.
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


#ifndef _PORTS_SAM_H
#define _PORTS_SAM_H 1

typedef u32 port_data_t;
typedef volatile port_data_t* port_t;

#define _define_port(NAME, PIO) \
    struct NAME { \
        static const u32 pio = u32(PIO); \
        static inline u32 port_input_read() { return ((Pio*)pio)->PIO_PDSR; } \
        static inline void port_output_write(u32 value) { \
            ((Pio*)pio)->PIO_ODSR = value; \
        } \
        static inline u32 port_output_read() { return ((Pio*)pio)->PIO_ODSR; } \
        static inline void port_enable_outputs(u32 mask) { PIO_Configure((Pio*)pio, PIO_OUTPUT_0, mask, PIO_DEFAULT); } \
        static inline void port_enable_inputs(u32 mask) { PIO_Configure((Pio*)pio, PIO_INPUT, mask, PIO_DEFAULT); } \
    }

#ifdef PIOA
_define_port(PORT_A, PIOA);
#endif

#ifdef PIOB
_define_port(PORT_B, PIOB);
#endif

#ifdef PIOC
_define_port(PORT_C, PIOC);
#endif

#ifdef PIOD
_define_port(PORT_D, PIOD);
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
        static inline void output_write(boolean value) { \
            if(value) { \
                ((Pio*)PORT::pio)->PIO_SODR = mask; \
            } else { \
                ((Pio*)PORT::pio)->PIO_CODR = mask; \
            } \
        } \
        static inline boolean output_read() { return (PORT::port_output_read() & mask) != 0; } \
    }

#define atomic for(boolean _loop_=(__disable_irq(),true);_loop_; _loop_=(__enable_irq(), false))

#if defined(ARDUINO_SAM_DUE)
#include "boards/sam/arduino_due_x.h"
#else
#warning "Unsupported Arduino SAM variant - falling back to digitalRead and digitalWrite."
#define DIRECTIO_FALLBACK 1
#endif 

#endif  // _PORTS_SAM_H
