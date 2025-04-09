void TestArm(void) {
  armReading = analogRead(ARMPIN);
  armReading = kfArm.updateEstimate(armReading);
  theta4 = mapping(armReading, 0, 1023, armLLim, armULim);
  //Serial.print("The potentiometer reading is ");
  //Serial.println(theta4);
  delay(30);
}
