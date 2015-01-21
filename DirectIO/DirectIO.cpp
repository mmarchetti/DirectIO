
#include <DirectIO.h>

InputPin::InputPin(u8 pin, boolean pullup) : 
	in_port(portInputRegister(digitalPinToPort(pin))),
	mask(digitalPinToBitMask(pin))
{ 
	pinMode(pin, pullup ? INPUT_PULLUP : INPUT);

	// include a call to digitalRead here which will 
	// turn off PWM on this pin, if needed
	(void) digitalRead(pin);
}

OutputPin::OutputPin(u8 pin, boolean initial_state): 
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
