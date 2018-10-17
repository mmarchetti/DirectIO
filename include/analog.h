/*
  analog.h - AnalogInput and AnalogOutput classes for Arduino.
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
