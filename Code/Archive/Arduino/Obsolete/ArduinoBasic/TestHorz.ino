void TestHorz(void) {
  horzReading = analogRead(HORZPIN);
  horzReading = kfHorz.updateEstimate(horzReading);
  d3 = mapping(horzReading, 0, 1023, horzLLim, horzULim);
  //Serial.print("The potentiometer reading is ");
  //Serial.println(d3);
  delay(30);
}
