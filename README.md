
### Contents
* [Why use DirectIO?](#user-content-why-use-directio)
* [Comparison](#user-content-comparison)
* [Performance](#user-content-performance)
* [API](#user-content-api)
  * [Input](#user-content-input)
  * [Output](#user-content-output)
  * [Multi-Bit I/O](#user-content-multi-bit-io)
    * [InputPort](#user-content-inputport)
    * [OutputPort](#user-content-outputport)
  * [Active Low Signals](#user-content-active-low-signals)
    * [InputLow](#user-content-inputlow)
    * [OutputLow](#user-content-outputlow)
  * [Pin Numbers Determined at Runtime](#user-content-pin-numbers-determined-at-runtime)
    * [InputPin](#user-content-inputpin)
    * [OutputPin](#user-content-outputpin)
  * [For Arduino IDE 1.0 Users](#user-content-for-arduino-ide-10-users)
  * [Supported Boards](#user-content-supported-boards)

### Why use DirectIO?
Two reasons: 
* Speed: on AVR boards, writes are 20x to 60x faster than the Arduino libraries. Maximum output frequency is 2.66 MHz, vs 64 KHz for the Arduino libraries. When reading or writing multiple I/O together, even greater performance gains are possible - over 200x for an 8-bit I/O port. On SAM-based systems (Due), writes are 17-40x faster than the Arduino libraries. Maximum frequency is 10.5 MHz vs. 237 KHz for the Arduino libraries.
* Simple API: just create pin objects. Assigning to a pin performs a write, using its value performs a read.

### Comparison
The standard Arduino I/O library (Wiring) isn't particularly fast. There are several contributing issues, but they primarily stem from pin numbers being specified at runtime. This can be useful, but for the most part, pin numbers are known at compile time. Key differences between native Arduino I/O and DirectIO include:

| Arduino I/O                |   DirectIO
|----------------------------|------------
| Pin numbers are checked for validity at runtime, on every I/O operation. | Validity checked at compile time, based on the target board selected in the Arduino IDE.
| I/O port address and bit mask are read from program memory on every I/O operation | Port and bit mask are determined at compile time, based on the target board.
| Since port addresses are loaded dynamically, indirect load instructions must be used. | Fast I/O instructions (`sbi` and `cbi`) are used by the compiler.
| Because of indirect addressing, digitalWrite must use a multi-instruction read/modify/write sequence. Since this is not atomic, digitalWrite must turn off interrupts and save/restore the status register. | `sbi` and `cbi` instructions execute atomically, so writes don't need to disable interrupts.
| digitalRead and digitalWrite check/disable PWM on the pin, on every I/O operation. | PWM on the pin is disabled at initialization time.

### Performance

#### AVR boards (Nano)

|                      | Arduino I/O (AVR)          | DirectIO (AVR)
| ---------------------|----------------------------|---------------------------|
| Init Code Size       | 6 bytes per I/O            | 12 bytes per I/O          |
| Input Code Size      | 6 bytes per read           | 2 bytes per read          |
| Output Code Size     | 6 bytes per write          | 2 bytes to write a constant<br>10 bytes to write a variable value |
| Time to Write Output | >120 cycles                | 2 cycles to write a constant<br>6 to 8 cycles to write a variable value |
| Max Output Frequency | 64 KHz                     | 2.66 MHz                  |
| RAM usage            | none                       | none                      |

[Benchmarks](extras/docs/avr_benchmarks.md)

#### SAM boards (Due)

|                      | Arduino I/O (SAM)          | DirectIO (SAM)            |
| ---------------------|----------------------------|---------------------------|
| Init Code Size       | 8 bytes per I/O            | 22 bytes per I/O          |
| Input Code Size      | 6 bytes per read           | 8 bytes per read          |
| Output Code Size     | 6 bytes per write          | 8 bytes to write a constant<br>14 bytes to write a variable value |
| Time to Write Output | >170 cycles | ~5 cycles to write a constant<br>~10 cycles to write a variable value |
| Max Output Frequency | 237 KHz                     | 10.5 MHz                 |
| RAM usage            | none                        | none                     |

[Benchmarks](extras/docs/arm_benchmarks.md)

### API

#### Input

Input is a class template; the template parameter is simply the pin number. You must specify a number that is known at compile time - a literal number, a number specified via `const u8 my_pin = ...`, or via `#define my_pin ...`.

```C++
template <u8 pin> class Input { ... };
```

For example, to create an input on pin 3 and read its value into a variable:

```C++
Input<3> my_input;
boolean value = my_input;           // implicit call to read()
boolean value2 = my_input.read();   // or use an explicit call, if you prefer
```

The Input constructor accepts an optional argument `pullup` specifying whether the port pullup resistors should be enabled.
```C++
Input<3> my_input(false);       // disable internal pullup on this input
```

#### Output

Simiar to Input, Output is a class template that requires a pin number to be specified.

```C++
template <u8 pin> class Output { ... };
```

For example, to create an output on pin 2 and turn it on:

```C++
Output<2> my_output;
my_output = HIGH;             // implicit call to write()
my_output.write(HIGH);        // or use an explicit call, if you prefer
```

The Output constructor accepts an optional argument `initial_value` specifying the initial state of the output (HIGH or LOW).

```C++
Output<2> my_output(HIGH);    // output should be initially set to HIGH
```

You can also read the current state of an output - no need to keep a separate state variable. Note that this reads back the value from the I/O port; no additional memory is used.

```C++
my_output = ! my_output;        // toggle the output
my_output = ! my_output.read(); // this works too, if you like explicit calls
my_output.toggle();             // or use the nice method provided
```

To emit a pulse of minimum duration (2 cycles, or 125 ns on a 16 Mhz board):

```C++
my_output.pulse(HIGH);          // set the output HIGH then LOW
```

or

```C++
my_output.pulse(LOW);           // set the output LOW then HIGH
```

#### Multi-Bit I/O

The Arduino standard library works hard to hide the implementation details of digital I/O, and presents a nicer API based on pin numbers. But there are advantages to breaking this model:
* Speed: you can read or write up to 8 pins with a single instruction (32 pins on SAM and SAMD boards).
* Simultaneity: all pins are read or written simultaneously.

The DirectIO library provides two classes (`InputPort` and `OutputPort`) that allow port-based I/O, mapping part or all of a processor port to a single port object.

1. Determine how many pins you need.
2. Look at the pinout for your Arduino variant, and identify a set of pins that share a common port and are sequentially numbered in that port. Or, look at the pin definitions for your board under `include/boards`.
3. Wire your project using those pins.
4. Define an `InputPort` or `OutputPort` object mapped to the selected port and pins. For example, support you are using a 4-bit port and have decided to use Port C2-C5 (in a standard Arduino sketch, these would be referred to as pins 16-19):

```C++
// Define a 4-bit port starting at port C2.
// This will control C2, C3, C4, C5 (pins 16-19).
OutputPort<PORT_C, 2, 4> my_port;

void setup() 
{
    my_port.setup();
}

void loop()
{
    // Turn on C2 (pin 16), and turn off the rest.
    my_port = 0x01;
}
```

Note the call to `my_port.setup()`; you must call `setup` on each port from your sketch's setup function. This is required for SAM/SAMD boards so that the pin configuration occurs after Arduino initialization.

##### InputPort

InputPort is a class template that takes 3 parameters:
* The port, as defined in `ports.h`. For example, `PORT_D`. Standard Arduinos use `PORT_D`, `PORT_B`, and `PORT_C`. Arduino Mega boards have ports up through `PORT_L`.
* The starting pin number in the port (default 0)
* The number of pins (default 8).

```
template <class port, u8 start_bit=0, u8 nbits=8> class InputPort { ... }
```

Like Input objects, InputPorts support reading values implicitly or explicitly:
```
InputPort<PORT_C, 2, 4> my_port;

void setup()
{
    my_port.setup();
}

void loop()
{
    u8 value = my_port;             // implicit call to read()
    u8 value2 = my_port.read();     // or use an explicit call, if you prefer
}
```

`read()` places the bits read from the port into the *n* low order bits of the returned value.

##### OutputPort

OutputPort is a class template. The parameters are the same as `InputPort`.

```
template <class port, u8 start_bit=0, u8 nbits=8> class OutputPort { ... }
```

Like Output objects, OutputPorts support writing values implicitly or explicitly:
```
OutputPort<PORT_C, 2, 4> my_port;

void setup()
{
    my_port.setup();
}

void loop()
{
    my_output = 0x07;             // implicit call to write()
    my_output.write(0x07);        // or use an explicit call, if you prefer
}
```

`read()` places the bits read from the port into the *n* low order bits of the returned value.

#### Active Low Signals

In some circuits, the meaning of inputs is reversed - for example, a switch input may be LOW when the switch is closed. This is an *active low* input. It can be helpful in program logic to consider LOW as true and HIGH as false. There are two classes that support active low signals.

##### InputLow

Define an active low input on pin 3 and read its value:

```C++
InputLow<3> switch;

if(switch) {
    // this code will execute when the switch is closed,
    // and the input voltage is low.
}
else {
    // this code will execute when the switch is open,
    // and the input voltage is high.
}
```


##### OutputLow

Suppose we have the cathode of an LED connected to pin 2 (the anode would be connected to +5V through a current limiting resistor). We can define an active low output on pin 2 to control it:

```C++
OutputLow<2> led;
led = true;       // turns on the LED by putting low voltage on pin 2
```

We could do the same thing with a normal Output if we didn't mind the backward logic:

```C++
Output<2> led;
led = false;      // turns on the LED by putting low voltage on pin 2
```

#### Pin Numbers Determined at Runtime

Like the easy to use syntax for reading and writing values, but have a case where you really don't know the pin number at compile time? For example, you might define a multi-pin output port and loop over a range of pin numbers writing values to each one. There are two classes that support this:

##### InputPin

```C++
boolean DoSomething(u8 pin)
{
    InputPin(pin) my_input;   // note pin number is now a constructor parameter
    return my_input;
}
```

`InputPin` looks up and caches the port address and bit mask (using 3 bytes of RAM per instance), in order to boost performance over digitalRead on AVR boards. On SAM/SAMD boards, this class currently delegates to `digitalRead`  so there is no speedup.

##### OutputPin

```C++
void DoSomething(u8 pin)
{
    OutputPin(pin) my_output; // note pin number is now a constructor parameter
    my_output = HIGH;
}
```

`OutputPin` looks up and caches the port address and bit mask (using 8 bytes of RAM per instance), in order to gain a 3x speedup over digitalWrite on AVR boards. On SAM/SAMD boards, this class currently delegates to `digitalWrite` so there is no speedup.

#### For Arduino IDE 1.0 Users
In order to map the pin numbers you specify into AVR ports, you need to tell the Direct IO library which Arduino board type you are using. If you are using Arduino IDE v1.5 or higher, the IDE will do this automatically based on the board selected in the Board menu. If you are using IDE 1.0, you will need to define which board you are using. For example, if you have an Uno board:
```C++
#define ARDUINO_AVR_UNO 1
#include <DirectIO.h>
```

If you omit this step, you will see a warning during compilation, and a standard Arduino board will be assumed:
``` 
error: #warning "Unsupported Arduino AVR variant. If you are using Arduino IDE 1.0, be sure to #define an Arduino variant (e.g. #define ARDUINO_AVR_UNO 1). See ports.h."
```

### Supported Boards
Supported Arduino variants include boards with AVR, SAM, and SAMD processors.

* AVR board variants:
```
ARDUINO_AVR_ADAFRUIT32U4
ARDUINO_AVR_ADK
ARDUINO_AVR_ATMEL_ATMEGA168PB_XMINI
ARDUINO_AVR_ATMEL_ATMEGA328PB_XMINI
ARDUINO_AVR_ATMEL_ATMEGA328P_XMINI
ARDUINO_AVR_BLUEFRUITMICRO
ARDUINO_AVR_BT
ARDUINO_AVR_CIRCUITPLAY
ARDUINO_AVR_DIGITAL_SANDBOX
ARDUINO_AVR_DUEMILANOVE
ARDUINO_AVR_EMORO_2560
ARDUINO_AVR_ESPLORA
ARDUINO_AVR_ETHERNET
ARDUINO_AVR_FEATHER328P
ARDUINO_AVR_FEATHER32U4
ARDUINO_AVR_FIO
ARDUINO_AVR_FIOV3
ARDUINO_AVR_FLORA8
ARDUINO_AVR_GEMMA
ARDUINO_AVR_INDUSTRIAL101
ARDUINO_AVR_ITSYBITSY32U4_3V
ARDUINO_AVR_ITSYBITSY32U4_5V
ARDUINO_AVR_LEONARDO
ARDUINO_AVR_LEONARDO_ETH
ARDUINO_AVR_LILYPAD
ARDUINO_AVR_LILYPAD_ARDUINO_USB_PLUS_BOARD
ARDUINO_AVR_LILYPAD_USB
ARDUINO_AVR_LININO_ONE
ARDUINO_AVR_MAKEYMAKEY
ARDUINO_AVR_MEGA
ARDUINO_AVR_MEGA2560
ARDUINO_AVR_METRO
ARDUINO_AVR_MICRO
ARDUINO_AVR_MINI
ARDUINO_AVR_NANO
ARDUINO_AVR_NG
ARDUINO_AVR_OLIMEXINO_328
ARDUINO_AVR_OLIMEXINO_32U4
ARDUINO_AVR_OLIMEXINO_85
ARDUINO_AVR_OLIMEXINO_Nano
ARDUINO_AVR_PRO
ARDUINO_AVR_PROMICRO
ARDUINO_AVR_PROTRINKET3
ARDUINO_AVR_PROTRINKET3FTDI
ARDUINO_AVR_PROTRINKET5
ARDUINO_AVR_PROTRINKET5FTDI
ARDUINO_AVR_QDUINOMINI
ARDUINO_AVR_RGB_GLASSES
ARDUINO_AVR_ROBOT_CONTROL
ARDUINO_AVR_ROBOT_MOTOR
ARDUINO_AVR_SERIAL_7_SEGMENT
ARDUINO_AVR_TRINKET3
ARDUINO_AVR_TRINKET5
ARDUINO_AVR_UNO
ARDUINO_AVR_UNO_WIFI_DEV_ED
ARDUINO_AVR_YUN
ARDUINO_AVR_YUNMINI
```

* SAM variants:

```
ARDUINO_SAM_DUE
```

* SAMD variants:

```
ARDUINO_SAMD_ADI
ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS
ARDUINO_SAMD_FEATHER_M0_EXPRESS
ARDUINO_SAMD_HALLOWING
ARDUINO_SAMD_INDUSTRUINO_D21G
ARDUINO_SAMD_MKR1000
ARDUINO_SAMD_MKRFox1200
ARDUINO_SAMD_MKRGSM1400
ARDUINO_SAMD_MKRWAN1300
ARDUINO_SAMD_MKRWIFI1010
ARDUINO_SAMD_MKRZERO
ARDUINO_SAMD_SMARTEVERYTHING_DRAGONFLY
ARDUINO_SAMD_SMARTEVERYTHING_FOX
ARDUINO_SAMD_SMARTEVERYTHING_LION
ARDUINO_SAMD_TIAN
ARDUINO_SAMD_ZERO
ARDUINO_SAM_ZERO
ADAFRUIT_METRO_M0_EXPRESS
ADAFRUIT_FEATHER_M0
SparkFun_SAMD21_Dev
SparkFun_SAMD_Mini
SparkFun_LilyMini
SparkFun_9DoF_M0
SparkFun_ProRF
```

SAMD testing was done on a `SparkFun_SAMD_Mini`. If you use a different board, please open an issue if you encounter any problems (and please report successful tests as well). All of the boards use the same code, but different pin mappings.

Other boards can be used in a fallback mode, with some limitations:
* DirectIO will not provide any acceleration. Internally, it will call `digitalRead` and `digitalWrite`.
* `InputPort` and `OutputPort` classes are not defined at this time.
