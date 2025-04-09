// Function to print out main menu
void printMenu(void) {
  Serial.println("a = I2C Scan\t\t--------- Scans for all connected I2C devices on pose bus and prints results.");
  Serial.println("b = LED Test\t\t--------- Blinks arm-top LED's until user input is entered.");
  Serial.println("c = Continuous Arm Read\t--------- Gives continuous readings for the 4 potentiometers on an arm.");
  Serial.println("d = Full Bus Read\t---------Prints readings for entire system for desired line");
  Serial.println("\nz = Print Menu");
  return;
}

// Function to clear Serial Monitor
void clearScreen(void) {
  for (int i = 0; i < 50; ++i) { // 'clears' screen and reprints menu
    Serial.println();
  }
}

// Menu Navigation Function
void menuSelect(char input) {
  Serial.print("User input was: "); Serial.println(input);

  switch (userInput) {
    case 'a':
      Serial.println("\nI2C Scanner");
      i2c_scan();
      break;

    case 'b':
      Serial.println("\nLED Test");
      test_LED();
      break;

    case 'c':
      Serial.println("\n Continous Arm Read");
      continuous_arm_read();
      break;

    case 'd':
      Serial.println("\n Full Bus Read");
      full_bus_read();
      break;

    default:
      Serial.println("Invalid input.\n");
      break;
  }

  while (Serial.available() > 0) { // Empty Serial Buffer
    Serial.read();
  }

  Serial.println("\nEnter anything to go back to menu.");
  while (Serial.available() < 2) {} // Wait for input

  while (Serial.available() > 0) { // Empty Serial Buffer
    Serial.read();
  }

  clearScreen();
  printMenu();
}
