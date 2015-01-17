#include <DirectIO.h>

Output<2> pin;

void setup() {}

void loop() {
  while(1) {
    pin = HIGH;
    pin = LOW;
  }
}

