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

#include <ports.h>

typedef volatile u8* port_t;

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
    public:
        InputPin(u8 pin, boolean pullup=true) : 
            pin(pin) { 
            pinMode(pin, pullup ? INPUT_PULLUP : INPUT); 
        }

        // Performance could be faster here if we bypass digitalRead, for example
        // by doing the lookup of the IO register and bit mask in the constructor
        // and using them in read()... at the cost of 2 bytes of RAM.
        boolean read() { 
            return digitalRead(pin); 
        }
        operator boolean() { 
            return read(); 
        }
	
	private:
		const u8 pin;
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

template <u8 pin> 
class Output {
	// An digital output with direct port I/O
    public:
        Output() { 
            pinMode(pin, OUTPUT); 
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

class OutputPin {
	// An digital output where the pin isn't known at compile time.
    public:
        OutputPin(u8 pin): 
            pin(pin) { 
            pinMode(pin, OUTPUT); 
        }

        // Performance could be faster here if we bypass digitalRead/digitalWrite, 
        // by doing the lookup of the IO register and bit mask in the constructor
        // and using them in write()... at the cost of a few bytes of RAM.
        
        void write(boolean value) { 
            digitalWrite(pin, value); 
        }
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
            return digitalRead(pin); 
        }
        operator boolean() { 
            return read(); 
        }
	
	private:
		const u8 pin;
};

template <u8 data_pin, u8 clock_pin, u8 nbits, class bits_t=u32, u8 bit_order=MSBFIRST> 
class InputShifter {
    public:
        InputShifter(bool pullup=true) : data(pullup) {}
        
		bits_t read() {
            bits_t value = 0;
            bits_t mask = (bit_order == LSBFIRST) ? 1 : (bits_t(1) << (nbits - 1));
            
            for(u8 i = 0; i < nbits; i++) {
                clock = HIGH;
                
                if(data) {
                    value |= mask;
                }
                clock = LOW;
                
                if (bit_order == LSBFIRST) {
                    mask <<= 1;
                }
                else {
                    mask >>= 1;
                }
            }
            return value;
        }

		operator bits_t() { 
            return read(); 
        }
		
	private:
		Input<data_pin> data;
		Output<clock_pin> clock;
};

template <u8 data_pin, u8 clock_pin, u8 nbits, class bits_t=u32, u8 bit_order=MSBFIRST> 
class OutputShifter {
    public:
        OutputShifter() {};
        
        void write(bits_t val) {
            bits_t mask = (bit_order == LSBFIRST) ? 1 : (bits_t(1) << (nbits - 1));
            
            for(u8 i = 0; i < nbits; i++) {
                data = (val & mask);
                clock.pulse();
                
                if (bit_order == LSBFIRST) {
                    mask <<= 1;
                }
                else {
                    mask >>= 1;
                }
            }
        }

        OutputShifter& operator = (bits_t val) { 
            write(val); 
            return *this; 
        }
        
    private:
		Output<data_pin> data;
		Output<clock_pin> clock;
};
