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

template <u8 data_pin, u8 clock_pin, u8 nbits, class bits_t=u32, u8 bit_order=MSBFIRST> 
class InputShifter {
	// A DirectIO implementation of shiftIn.
    public:
        InputShifter(bool pullup=true) : data(pullup) {}
        
		bits_t read() {
			// read nbits bits from the input pin and pack them
			// into a value of type bits_t. If nbits is <= 16,
			// bits_t can be set to u16 for better performance. 
			// If nbits <= 8, use u8.
			
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
	// A DirectIO implementation of shiftOut.
    public:
        OutputShifter() {};
        
        void write(bits_t val) {
			// write nbits bits to the output pin. If nbits is <= 16,
			// bits_t can be set to u16 for better performance. 
			// If nbits <= 8, use u8.
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
