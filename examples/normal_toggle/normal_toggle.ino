#define PIN 2

void setup() {
    pinMode(PIN, OUTPUT);
}

void loop() {
  boolean val = HIGH;
  while(1) {
    digitalWrite(PIN, val);
    val = ! val;
  }
}

