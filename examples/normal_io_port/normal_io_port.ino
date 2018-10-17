#define FIRST_PIN 0

void setup() 
{
  for(uint8_t i = 0; i < 8; i++) {
   pinMode(FIRST_PIN + i, OUTPUT);
  }
}

void loop() {
  uint8_t value = 0;
  
  while(1) {
    for(uint8_t i = 0; i < 8; i++) {
     digitalWrite(FIRST_PIN + 7 - i, bitRead(value, i));
    }
    value++;
  }
}
