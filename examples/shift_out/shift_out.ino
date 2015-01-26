#include <DirectIO.h>
#include "DirectIO_Shift.h"

// define a shift register with data on pin 2 and clock on pin 3,
// and storage register clock on pin 4.
// output size is 8 bits in this example.
ShiftRegister595<2, 3, 4> out;

void setup() {}

void loop() {
  // every time through the loop we write a unique value
  static u8 i = 0;
  
  out = i;
  i++;
}
