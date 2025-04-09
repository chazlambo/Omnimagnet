void TestVert(void) {
  vertReading = analogRead(VERTPIN);
  vertReading = kfVert.updateEstimate(vertReading);
  d1 = mapping(vertReading, 0, 1023, vertLLim, vertULim);
  //Serial.print("The potentiometer reading is ");
  //Serial.println(d1);
  delay(30);
}
