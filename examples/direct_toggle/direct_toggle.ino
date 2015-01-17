#include <DirectIO.h>

Output<2> out;

void setup() {}

void loop() {
  while(1) {
    out.toggle();
  }
}

