/*
  DirectIO_Shift.h - Bit shift in/out using Direct IO library
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

#include <DirectIO.h>

template <u8 data_pin, u8 clock_pin, u8 nbits, u8 bit_order=MSBFIRST> 
class ClockedInput {
	// A DirectIO implementation of shiftIn. Also supports 
	// a variable number of bits (1-32); shiftIn is always 8 bits.
    public:
		// Define a type large enough to hold nbits bits (see base.h)
		typedef bits_type(nbits) bits_t;
		
        ClockedInput(bool pullup=true) : data(pullup) {}
        
		bits_t read() {
			// read nbits bits from the input pin and pack them
			// into a value of type bits_t.
			
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

template <u8 data_pin, u8 clock_pin, u8 nbits, u8 bit_order=MSBFIRST> 
class ClockedOutput {
	// A DirectIO implementation of shiftOut. Also supports 
	// a variable number of bits (1-32); shiftOut is always 8 bits.
    public:
		// Define a type large enough to hold nbits bits (see base.h)
		typedef bits_type(nbits) bits_t;
		
        ClockedOutput() {};
        
        void write(bits_t val) {
			// write nbits bits to the output pin
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

        ClockedOutput& operator = (bits_t val) { 
            write(val); 
            return *this; 
        }
        
    private:
		Output<data_pin> data;
		Output<clock_pin> clock;
};

template <u8 data_pin, u8 shift_clock_pin, u8 storage_clock_pin, 
	u8 output_enable_pin=NO_PIN, u8 nbits=8, u8 bit_order=MSBFIRST> 
class ShiftRegister595 {
	// ShiftRegister595 models a serial-in parallel-out shift register
	// such as the 74HC595 (or up to 4 of them connected in series).
	// The process for putting values onto the output pins is:
	// For each bit, write the bit onto the data pin and pulse the 
	// shift register clock (this is done by ClockedOutput).
	// When all bits have been transferred, pulse the storage
	// register clock to transfer all bits at once to the storage
	// register and output pins.
	// Note that the Output Enable pin is optional; if omitted,
	// then no Output Enable pin will be used. This is useful
	// if you have wired the 595's OE pin to Vcc.
	
	public:
		// define a type just large enough to hold nbits bits
		typedef bits_type(nbits) bits_t;
		
		ShiftRegister595(boolean enabled=true) : output_enable(enabled) {}
		
		void write(bits_t val) {
			// shift bits into the shift register
			shift_reg.write(val);
			
			// shift register contents will be transferred to the 
			// storage register and output pins on the positive
			// edge of the storage clock
			storage_clock.pulse(HIGH);
		}
		
        ShiftRegister595& operator = (bits_t val) { 
            write(val); 
            return *this; 
        }
		
		void enable() {
			output_enable = HIGH;
		}
		
		void disable() {
			output_enable = LOW;
		}
	
	private:
		ClockedOutput<data_pin, shift_clock_pin, nbits, bit_order> shift_reg;
		Output<storage_clock_pin> storage_clock;
		Output<output_enable_pin> output_enable;
};
