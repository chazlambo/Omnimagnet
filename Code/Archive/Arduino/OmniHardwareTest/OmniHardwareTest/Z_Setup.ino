void setupFun(void){
  // Initialize Serial Communication
  Serial.begin(115200);

  // Setup Functions
  setup_I2C();
  
  clearScreen();  // Clear Serial Monitor
  printMenu();    // Print Menu  

}
