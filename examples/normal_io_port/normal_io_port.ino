#define FIRST_PIN 0

void setup() 
{
  for(u8 i = 0; i < 8; i++) {
   pinMode(FIRST_PIN + i, OUTPUT);
  }
}

void loop() {
  u8 value = 0;
  
  while(1) {
    for(u8 i = 0; i < 8; i++) {
     digitalWrite(FIRST_PIN + 7 - i, bitRead(value, i));
    }
    value++;
  }
}

