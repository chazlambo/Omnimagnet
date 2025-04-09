void continuous_arm_read() {
  \
  // Create variables for user inputs
  char busChar;
  char armChar;

  Serial.println("\nInput desired I2C Bus:");
  Serial.println("\t1 - Pose");
  Serial.println("\t2 - Temperature");

  while (Serial.available() < 2); {
  }
  busChar = Serial.read(); // Read input

  while (Serial.available()) {
    Serial.read();  // Remove rest of serial
  }

  int busSwitch = busChar - '0';  // Convert input character to integer

  // Check for valid input
  if (!(busSwitch == 1 || busSwitch == 2)) {
    Serial.println("\n\nERROR:");
    Serial.println("Input must be either 1 or 2.\n");
    return;
  }

  Serial.println("\nInput Desired Arm #:");
  Serial.println("\t1 - Arm 1");
  Serial.println("\t2 - Arm 2");
  Serial.println("\t3 - Arm 3");
  Serial.println("\t4 - Arm 4");
  Serial.println("\t5 - Arm 5");

  while (Serial.available() < 2); { // Wait for serial input
  }

  armChar = Serial.read();  // Read input

  while (Serial.available()) {
    Serial.read();  // Remove rest of serial
  }

  int armSwitch = armChar - '0'; // Convert input character to integer

  // Check for valid input
  if (!(armSwitch == 1 || armSwitch == 2 || armSwitch == 3 || armSwitch == 4 || armSwitch == 5)) {
    Serial.println("\n\nERROR:");
    Serial.println("Input must be either 1 or 2.\n");

    return; // Exit function if invalid input
  }


  if (busSwitch == 1) {
    Serial.print("Position ");
  }
  else if (busSwitch == 2) {
    Serial.print("Temperature ");
  }
  Serial.print(" read for Arm #"); Serial.print(armChar); Serial.println(" starting soon.\n\nEnter any character during runtime to exit");

  armCommStart(armSwitch);
  
  while (!Serial.available()) {
    printArm(busSwitch, armSwitch);
  }
  armCommEnd(armSwitch);

}

void printArm(int bus, int arm) {
  Serial.print("\nArm #"); Serial.print(arm); Serial.print(" - ");
  
  if (bus == 1) {
    Serial.println("Position");
    Serial.println("\tA0\tA1\tA2\tA3");
    Serial.println("\tWrist\tHorz\tBase\tVert");
    switch (arm) {
      case 1:
        adcPrint(arm1_pose, bus, arm);
        break;

      case 2:
        adcPrint(arm2_pose, bus, arm);
        break;

      case 3:
        adcPrint(arm3_pose, bus, arm);
        break;

      case 4:
        adcPrint(arm4_pose, bus, arm);
        break;

      case 5:
        adcPrint(arm5_pose, bus, arm);
        break;
    }
  }
  else if (bus == 2) {
    ;
    Serial.println("Temperature");
    Serial.println("\tA0\tA1\tA2\tA3(ignore)");
    switch (arm) {
      case 1:
        adcPrint(arm1_temp, bus, arm);
        break;

      case 2:
        adcPrint(arm2_temp, bus, arm);
        break;

      case 3:
        adcPrint(arm3_temp, bus, arm);
        break;

      case 4:
        adcPrint(arm4_temp, bus, arm);
        break;

      case 5:
        adcPrint(arm5_temp, bus, arm);
        break;
    }
  }
}

void full_bus_read(void) {
  char busChar;

  Serial.println("\nInput desired I2C Bus:");
  Serial.println("\t1 - Pose");
  Serial.println("\t2 - Temperature");

  while (Serial.available() < 2); // Wait for user input
  busChar = Serial.read();

  while(Serial.available()){
    Serial.read();
  }

  int busSwitch = busChar - '0';  // Convert character to integer

  // Check for valid input
  if (!(busSwitch == 1 || busSwitch == 2)) {
    Serial.println("\n\nERROR:");
    Serial.println("Input must be either 1 or 2.\n");
    return;
  }

  printBus(busSwitch);
}

void printBus(int bus) {

  for (int i=1; i<5; i++) {
    armCommStart(i);
  }
  
  if (bus == 1){
    Serial.println("Printing full system position readings.");
    Serial.println("Arm #\tHorz.\t Wrist\tBase \tVert.");
    Serial.print(" 1 "); adcPrint(arm1_pose, 1, 1);
    Serial.print(" 2 "); adcPrint(arm1_pose, 1, 2);
    Serial.print(" 3 "); adcPrint(arm1_pose, 1, 3);
    Serial.print(" 4 "); adcPrint(arm1_pose, 1, 4);
    Serial.print(" 5 "); adcPrint(arm1_pose, 1, 5);
  }

  if (bus == 2){
    Serial.println("Printing full system temperature readings. (Ignore 4th column)");
    Serial.println("Arm #\tA0 \tA1 \tA2 \tNULL");
    Serial.print(" 1 "); adcPrint(arm1_pose, 1, 1);
    Serial.print(" 2 "); adcPrint(arm1_pose, 1, 2);
    Serial.print(" 3 "); adcPrint(arm1_pose, 1, 3);
    Serial.print(" 4 "); adcPrint(arm1_pose, 1, 4);
    Serial.print(" 5 "); adcPrint(arm1_pose, 1, 5);
  }

  for (int i=1; i<5; i++) {
    armCommEnd(i);
  }
}
