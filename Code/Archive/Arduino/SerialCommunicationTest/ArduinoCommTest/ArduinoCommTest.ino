#include "Servo.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Arduino is ready!");
}

void loop() {
  if(Serial.available()) {
    char c = Serial.read();
    Serial.print("Arduino reads: ");
    Serial.println(c);
  }
}
