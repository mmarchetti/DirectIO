#include <DirectIO.h>

OutputPin pin(2);

void setup() {}

void loop() {
  while(1) {
    pin = HIGH;
    pin = LOW;
  }
}

