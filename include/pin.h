/*
  pin.h - InputPin and OutputPin classes for AVR and other Arduino variants.
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

#if defined(ARDUINO_ARCH_AVR)
class InputPin {
    // An digital input where the pin isn't known at compile time.
    // We cache the port address and bit mask for the pin
    // and read() reads directly from port memory.
    public:
        InputPin(u8 pin, boolean pullup=true);

        boolean read() {
            return (*in_port & mask) != 0;
        }
        operator boolean() {
            return read();
        }

    private:
        port_t      in_port;
        port_data_t mask;
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
            return (*in_port & on_mask) != 0;
        }
        operator boolean() {
            return read();
        }

    private:
        port_t       in_port;
        port_t       out_port;
        port_data_t  on_mask;
        port_data_t  off_mask;
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

#else  // ARDUINO_ARCH_AVR

class InputPin {
    // An digital input where the pin isn't known at compile time.
    // We cache the port address and bit mask for the pin
    // and read() reads directly from port memory.
    public:
        InputPin(u8 pin, boolean pullup=true);

        boolean read() {
            return digitalRead(pin);
        }
        operator boolean() {
            return read();
        }

    private:
        u8 pin;
};

class OutputPin {
    // An digital output where the pin isn't known at compile time.
    // We cache the port address and bit mask for the pin
    // and write() writes directly to port memory.
    public:
        OutputPin(u8 pin, boolean initial_value=LOW);

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
        u8 pin;
};

inline InputPin::InputPin(u8 pin, boolean pullup):
    pin(pin)
{
    pinMode(pin, pullup ? INPUT_PULLUP : INPUT);

    // include a call to digitalRead here which will
    // turn off PWM on this pin, if needed
    (void) digitalRead(pin);
}

inline OutputPin::OutputPin(u8 pin, boolean initial_state):
    pin(pin)
{
    pinMode(pin, OUTPUT);

    // include a call to digitalWrite here which will
    // set the initial state and turn off PWM
    // on this pin, if needed.
    digitalWrite(pin, initial_state);
}

#endif  // ARDUINO_ARCH_AVR
