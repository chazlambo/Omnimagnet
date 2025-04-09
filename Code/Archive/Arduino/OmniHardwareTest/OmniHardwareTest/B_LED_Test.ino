void test_LED() {
  Serial.println("\nEnter anything to exit program");

  tcaselect(1); // Set multiplexer to position port
  
  while (Serial.available() == 0){
    //Turn on Green Light
    arm1_pose.analogWrite(255);
    arm2_pose.analogWrite(255);
    arm3_pose.analogWrite(255);
    arm4_pose.analogWrite(255);
    arm5_pose.analogWrite(255);
    delay(1000);
    
    // Turn off Green Light
    arm1_pose.analogWrite(0);
    arm2_pose.analogWrite(0);
    arm3_pose.analogWrite(0);
    arm4_pose.analogWrite(0);
    arm5_pose.analogWrite(0);
    delay(1000);
  }

  while (Serial.available() > 0) {
    Serial.read();
  }

  tcaselect(0);
  
  return;
}
