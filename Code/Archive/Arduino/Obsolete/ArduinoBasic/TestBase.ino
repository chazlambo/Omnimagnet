void TestBase(void) {
  baseReading = analogRead(BASEPIN);
  baseReading = kfBase.updateEstimate(baseReading);
  theta1 = mapping(baseReading,0,1023,baseLLim,baseULim);
  //Serial.print("The potentiometer reading is ");
  delay(30);
}
