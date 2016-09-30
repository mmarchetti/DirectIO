/*
  DirectIO.h - Main include file for Direct IO library
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

#ifndef _DIRECTIO_H
#define _DIRECTIO_H 1

#include <ports.h>

typedef volatile u8* port_t;
const u8 NO_PIN = 255;

template <u8 pin> 
class Input {
	// An standard digital input. read() returns true if the signal is asserted (high).
    public:
        Input(boolean pullup=true) { 
            pinMode(pin, pullup ? INPUT_PULLUP : INPUT); 
        }
        boolean read() { 
            return bitRead(*port_t(_pins<pin>::in), _pins<pin>::bit); 
        }
        operator boolean() { 
            return read(); 
        }
};

class InputPin {
	// An digital input where the pin isn't known at compile time.
	// We cache the port address and bit mask for the pin
	// and read() reads directly from port memory.
    public:
        InputPin(u8 pin, boolean pullup=true);

        boolean read() { 
            return *in_port & mask;
        }
        operator boolean() { 
            return read(); 
        }
	
	private:
		port_t	in_port;
		u8		mask;
};

template <u8 pin> 
class InputLow {
	// An active low digital input. read() returns true if the signal is asserted (low).
    public:
        InputLow() {}
        boolean read() { 
            return ! input.read(); 
        }
        operator boolean() { 
            return read(); 
        }
        
    private:
        Input<pin> input;
};

// This macro lets you temporarily set an output to a value, 
// and toggling back at the end of the code block. For example:
// 
// Output<2> cs;
// Output<3> data;
// with(cs, LOW) {
//     data = HIGH;
// }
// 
// is equivalent to:
// cs = LOW;
// data = HIGH;
// cs = HIGH;

#define with(pin, val) for(boolean _loop_##pin=((pin=val),true);_loop_##pin; _loop_##pin=((pin=!val), false))

template <u8 pin> 
class Output {
	// An digital output with direct port I/O
    public:
        Output(boolean initial_value=LOW) { 
            pinMode(pin, OUTPUT); 
			
			// include a call to digitalWrite here which will 
			// turn off PWM on this pin, if needed
			digitalWrite(pin, initial_value); 
       }
        void write(boolean value) {
            bitWrite(*port_t(_pins<pin>::out), _pins<pin>::bit, value); 
        }
        Output& operator =(boolean value) { 
            write(value); 
            return *this; 
        }
        void toggle() { 
            write(! read()); 
        }
        void pulse(boolean value=HIGH) { 
            write(value); 
            write(! value); 
        }
        boolean read() { 
            return bitRead(*port_t(_pins<pin>::in), _pins<pin>::bit); 
        }
        operator boolean() { 
            return read(); 
        }
};

template <> 
class Output<NO_PIN> {
	// This specialization of Output is used when a module supports
	// an optional pin and that pin is not being used. For example,
	// if a chip supports an Output Enable pin but it is wired
	// permanently HIGH in the circuit, there won't be an actual
	// output pin connected to it. In the software, we will use this
	// type of Output which is basically a no-op.
    public:
        Output(boolean initial_value=LOW) {}
        void write(boolean value) {}
        Output& operator =(boolean value) { 
            return *this; 
        }
        void toggle() {}
        void pulse(boolean value=HIGH) {}
        
        boolean read() { 
            return LOW;
        }
        operator boolean() { 
            return read(); 
        }
};

class OutputPin {
	// An digital output where the pin isn't known at compile time.
	// We cache the port address and bit mask for the pin
	// and write() writes directly to port memory.
    public:
        OutputPin(u8 pin, boolean initial_value=LOW);

        void write(boolean value);
        OutputPin& operator =(boolean value) { 
            write(value); 
            return *this; 
        }
        void toggle() { 
            write(! read()); 
        }
        void pulse(boolean value=HIGH) { 
            write(value);
            write(! value); 
        }
        boolean read() { 
            return *in_port & on_mask;
        }
        operator boolean() { 
            return read(); 
        }
	
	private:
		port_t	in_port;
		port_t	out_port;
		u8		on_mask;
		u8		off_mask;
};

inline InputPin::InputPin(u8 pin, boolean pullup) : 
	in_port(portInputRegister(digitalPinToPort(pin))),
	mask(digitalPinToBitMask(pin))
{ 
	pinMode(pin, pullup ? INPUT_PULLUP : INPUT);

	// include a call to digitalRead here which will 
	// turn off PWM on this pin, if needed
	(void) digitalRead(pin);
}

inline OutputPin::OutputPin(u8 pin, boolean initial_state): 
	in_port(portInputRegister(digitalPinToPort(pin))),
	out_port(portOutputRegister(digitalPinToPort(pin))),
	on_mask(digitalPinToBitMask(pin)),
	off_mask(~on_mask)
{
	pinMode(pin, OUTPUT);

	// include a call to digitalWrite here which will 
	// set the initial state and turn off PWM 
	// on this pin, if needed.
	digitalWrite(pin, initial_state);
}

inline void OutputPin::write(boolean value) 
{ 
	atomic {
        if(value) {
            *out_port |= on_mask;
        }
        else {
            *out_port &= off_mask;
        }
    }
}

template <class port, u8 start_bit=0, u8 nbits=8>
class InputPort {
	// A set of digital inputs which are contiguous and
	// located in a single MCU I/O port. This abandons
	// the pin number model for specifying I/O pins,
	// in order to gain fast, simultaneous 
	// multi-bit reads and writes.
    public:
		InputPort() {}
		
		u8 read() {
			// mask to select bits of interest, then shift so
			// that output can be treated as normal integers.
			return (*port_t(port::in) & mask) >> start_bit; 
		}
		operator u8() { 
			return read(); 
		}
		
	private:
		static const u8 mask = ((u8(1) << nbits) - 1) << start_bit;
};

template <class port, u8 start_bit=0, u8 nbits=8>
class OutputPort {
	// A set of digital outputs which are contiguous and
	// located in a single MCU I/O port. This abandons
	// the pin number model for specifying I/O pins,
	// in order to gain fast, simultaneous 
	// multi-bit reads and writes.
    public:
		OutputPort() {
			// set port pin directions to output
			atomic {
				*port_t(port::dir) |= mask;
			}
		}
		
        void write(u8 value) {
			atomic {
                // read-modify-write cycle
				u8 v = *port_t(port::in);
				u8 shifted = value << start_bit;
				v |= shifted & mask;
				v &= (shifted | ~mask);
				*port_t(port::out) = v;
			}
        }
        OutputPort& operator =(u8 value) { 
            write(value); 
            return *this; 
        }
		u8 read() {
			// mask to select bits of interest, then shift so
			// that output can be treated as normal integers.
			return (*port_t(port::in) & mask) >> start_bit; 
		}
		operator u8() { 
			return read(); 
		}
		
	private:
		static const u8 mask = ((u8(1) << nbits) - 1) << start_bit;
};

template <class port>
class OutputPort<port, 0, 8> {
	// Specialization for a complete MCU output port.
	// This simplifies writes, which no longer require
	// a read/modify/write cycle. This reduces the
	// bit manipulation required, and also eliminates
	// the need to disable/reenable interrupts during writes.
    public:
		OutputPort() {
			// set port pin directions to output
			*port_t(port::dir) = 0xFF;
		}
		
        void write(u8 value) {
			*port_t(port::out) = value;
        }
        OutputPort& operator =(u8 value) { 
            write(value); 
            return *this; 
        }
		u8 read() {
			return *port_t(port::in); 
		}
		operator u8() { 
			return read(); 
		}
};

template<u8 pin>
class AnalogInput {
    public:
        AnalogInput() {}
        
        u16 read() {
            return analogRead(pin);
        }
        
        operator u16 () {
            return read();
        }
};

template<u8 pin>
class AnalogOutput {
    public:
        AnalogOutput(u8 initial_value=0) {
            write(initial_value);
        }
        
        void write(u8 value) {
            analogWrite(pin, value);
        }
        
        AnalogOutput& operator = (u8 value) {
            write(value);
            return *this;
        }
};

template<>
class AnalogOutput<NO_PIN> {
    // This specialization of AnalogOutput is used when a module supports
    // an optional pin and that pin is not being used. For example,
    // the sample LCD project includes a backlight pin which can optionally
    // be driven with PWM. Alternatively, it could be wired to Vcc to keep
    // the backlight fully on. In that case, we will use this
    // type of AnalogOutput which is basically a no-op.
    public:
        AnalogOutput(u8 initial_value=0) {}
        void write(u8 value) {}
        
        AnalogOutput& operator = (u8 value) {
            return *this;
        }
};

template<u8 pin>
class AnalogOutputLow {
    public:
        AnalogOutputLow(u8 initial_value=0) {
            write(initial_value);
        }
        
        void write(u8 value) {
            analogWrite(pin, 255 - value);
        }
        
        AnalogOutputLow& operator = (u8 value) {
            write(value);
            return *this;
        }
};

template<>
class AnalogOutputLow<NO_PIN> {
    // This specialization of AnalogOutputLow is used when a module supports
    // an optional pin and that pin is not being used.
    // See AnalogOutput<NO_PIN> for details.
    public:
        AnalogOutputLow(u8 initial_value=0) {}
        void write(u8 value) {}
        
        AnalogOutputLow& operator = (u8 value) {
            return *this;
        }
};

#endif // _DIRECTIO_H
