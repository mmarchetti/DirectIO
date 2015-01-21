
## Why use DirectIO?
Two reasons: 
* Speed: writes are 40x to 60x faster than the Arduino libraries. Maximum output frequency is 2.66 MHz, vs 64 KHz for the Arduino libraries.
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

|                      | Arduino I/O                |   DirectIO                |
| ---------------------|----------------------------|---------------------------|
| Init Code Size       | 6 bytes per I/O            | 12 bytes per I/O          |
| Input Code Size      | 6 bytes per read           | 2 bytes per read          |
| Output Code Size     | 6 bytes per write          | 2 bytes to write a constant<br>10 bytes to write a variable value |
| Time to Write Output | >120 cycles                | 2 cycles to write a constant<br>6 to 8 cycles to write a variable value |
| Max Output Frequency | 64 KHz                     | 2.66 MHz                  |
| RAM usage            | none                       | none                      |

### API

#### Include Files
When you add the DirectIO library to your project, the Arduino IDE will add all of the include files:
```C++
#include <base.h>
#include <DirectIO.h>
#include <ports.h>
```
Remove `base.h` and `ports.h`, and just include `DirectIO.h`.

#### For Arduino IDE 1.0 Users
In order to map the pin numbers you specify into AVR ports, you need to tell the Direct IO library which Arduino board type you are using. If you are using Arduino IDE v1.5 or higher, the IDE will do this automatically based on the board selected in the Board menu. If you are using IDE 1.0, you will need to define which board you are using. For example, if you have an Uno board:
```C++
#define ARDUINO_AVR_UNO 1
#include <DirectIO.h>
```

If you omit this step, you will see this compilation error:
``` 
error: #error "Unsupported Arduino variant. If you are using Arduino IDE 1.0, be sure to #define an Arduino variant (e.g. #define ARDUINO_AVR_UNO 1). See ports.h."
```

There are three supported Arduino variants:
* Standard board variants:
```
	ARDUINO_AVR_UNO
	ARDUINO_AVR_YUN
	ARDUINO_AVR_DUEMILANOVE
	ARDUINO_AVR_NANO
	ARDUINO_AVR_MINI
	ARDUINO_AVR_ETHERNET
	ARDUINO_AVR_FIO
	ARDUINO_AVR_BT
	ARDUINO_AVR_LILYPAD
	ARDUINO_AVR_PRO
	ARDUINO_AVR_NG
```

* Mega board variants:
```
	ARDUINO_AVR_MEGA2560
	ARDUINO_AVR_ADK
```

* Leonardo board variants:
```
	ARDUINO_AVR_LEONARDO
	ARDUINO_AVR_MICRO
	ARDUINO_AVR_ESPLORA
	ARDUINO_AVR_LILYPAD_USB
	ARDUINO_AVR_ROBOT_MOTOR
	ARDUINO_AVR_ROBOT_CONTROL
```

*Note, the Arduino Due (ARM platform) isn't supported yet.*

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
my_output = HIGH;           	// implicit call to write()
my_output.write(HIGH);      	// or use an explicit call, if you prefer
```

The Output constructor accepts an optional argument `initial_value` specifying the initial state of the output (HIGH or LOW).

```C++
Output<2> my_output(HIGH);   	// output should be initially set to HIGH
```

You can also read the current state of an output - no need to keep a separate state variable. Note that this reads back the value from the I/O port; no additional memory is used.

```C++
my_output = ! my_output;		// toggle the output
my_output = ! my_output.read();	// this works too, if you like explicit calls
my_output.toggle();				// or use the nice method provided
```

To emit a pulse of minimum duration (2 cycles, or 125 ns on a 16 Mhz board):

```C++
my_output.pulse(HIGH);			// set the output HIGH then LOW
```

or

```C++
my_output.pulse(LOW);			// set the output LOW then HIGH
```

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
led = true;				// turns on the LED by putting low voltage on pin 2
```

We could do the same thing with a normal Output if we didn't mind the backward logic:

```C++
Output<2> led;
led = false;			// turns on the LED by putting low voltage on pin 2
```

#### Pin Numbers Determined at Runtime

Like the easy to use syntax for reading and writing values, but have a case where you really don't know the pin number at compile time? For example, you might define a multi-pin output port and loop over a range of pin numbers writing values to each one. There are two classes that support this:

##### InputPin

```C++
boolean DoSomething(u8 pin)
{
	InputPin(pin) my_input;		// note pin number is now a constructor parameter
	return my_input;
}
```

`InputPin` looks up and caches the port address and bit mask (using 3 bytes of RAM per instance), in order to boost performance over digitalRead.

##### OutputPin

```C++
void DoSomething(u8 pin)
{
	OutputPin(pin) my_output;	// note pin number is now a constructor parameter
	my_output = HIGH;
}
```

`OutputPin` looks up and caches the port address and bit mask (using 8 bytes of RAM per instance), in order to gain a 10x speedup over digitalWrite.

### Benchmark: Arduino I/O

Here's a short sketch that drives an output pin as fast as possible:  
  
```C++
#define PIN 2  

void setup() {  
    pinMode(PIN, OUTPUT);  
}  

void loop() {  
  while(1) {  
    digitalWrite(PIN, HIGH);  
    digitalWrite(PIN, LOW);  
  }  
}  
```

This generates the following code:  
```
00000234 <setup>:  
 234:   61 e0           ldi r22, 0x01   ; 1  
 236:   82 e0           ldi r24, 0x02   ; 2  
 238:   5a c0           rjmp    .+180       ; 0x2ee <pinMode>  
```

In this loop, each write to the output requires 3 instructions to set up a call to `digitalWrite`. 
  
```
0000023a <loop>:
 23a:   61 e0           ldi r22, 0x01   ; 1
 23c:   82 e0           ldi r24, 0x02   ; 2
 23e:   90 d0           rcall   .+288       ; 0x360 <digitalWrite>
 
 240:   60 e0           ldi r22, 0x00   ; 0
 242:   82 e0           ldi r24, 0x02   ; 2
 244:   8d d0           rcall   .+282       ; 0x360 <digitalWrite>
 
 246:   f9 cf           rjmp    .-14        ; 0x23a <loop>
```

Each pass through the loop takes 250 cycles. On a 16 Mhz board, this gives an output frequency of 64 KHz.

![Trace of Arduino IO case](normal.png)

### Benchmark: Direct I/O

Here's the same loop, using the DirectIO library:

```C++
#include <DirectIO.h>  

Output<2> pin;  

void setup() {}  

void loop() {  
  while(1) {  
    pin = HIGH;  
    pin = LOW;  
  }  
}  
```

setup() is now empty, and the initialization is done in the constructor of the global variable 'pin':
```
00000254 <setup>:
 254:   08 95           ret

0000025c <_GLOBAL__sub_I_pin>:
 25c:   61 e0           ldi r22, 0x01   ; 1
 25e:   82 e0           ldi r24, 0x02   ; 2
 260:   56 d0           rcall   .+172       ; 0x30e <pinMode>
 
 262:   60 e0           ldi r22, 0x00   ; 0
 264:   82 e0           ldi r24, 0x02   ; 2
 266:   8c c0           rjmp    .+280       ; 0x380 <digitalWrite>
```

In the new loop, each write to the output is a single instruction. This is what makes the DirectIO library so fast.

```
00000256 <loop>:
 256:   74 9a           sbi 0x0e, 4 ; 14
 258:   74 98           cbi 0x0e, 4 ; 14
 25a:   fd cf           rjmp    .-6         ; 0x256 <loop>
```

Each pass through the loop takes 6 cycles; on a 16 Mhz board, this
gives an output frequency of 2.66 MHz - over 40x faster than the native
Arduino I/O.

![Trace of Direct IO case](direct.png)

### Benchmark: Direct I/O with Dynamic Pin Number

One more time, using pin numbers specified at runtime. Note that you should only do this if you need dynamic pin numbering; if you have constant pin numbers, use the `Output` class described above.

```C++
#include <DirectIO.h>  

OutputPin pin(2);  

void setup() {}  

void loop() {  
  while(1) {  
    pin = HIGH;  
    pin = LOW;  
  }  
}  
```

Each pass through the loop takes 75 cycles; on a 16 Mhz board, this
gives an output frequency of 214 KHz - over 3x faster than the native
Arduino I/O.

![Trace of Direct IO dynamic case](direct_pin.png)
