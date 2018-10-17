#include <DirectIO.h>

OutputPort<PORT_D> port;

void setup() {
  port.setup();
}

void loop() {
  u8 i = 0;
  
  while(1) {
    port = i++;
  }
}
