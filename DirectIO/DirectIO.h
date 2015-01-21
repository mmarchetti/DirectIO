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
