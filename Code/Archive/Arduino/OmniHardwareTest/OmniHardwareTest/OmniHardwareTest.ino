/****************************
   Omnimagnet Hardware Test
   A program to test all the hardware on the Omnimagnet System

 ** Tab Labeling **
 * For better code clarity the code has been separated into tabs.
 * Arduino compiles tabs Alphabetically so the following labels are used to ensure correct compilation
 * A - Variable Declarations, Setup and global functions
 * B - Main Functions
 * Z - Final Setup Function
   VERSION: 1.0
   Date: 9/28/2022
   
 * * Written by Charlie Lambert
 ****************************/


/*******************
 **   #includes   **
 *******************/
#include <Wire.h>                 // Basic I2C Wire Library for Arduino
#include <Adafruit_PCF8591.h>     // Library for the ADC Modules

/**********************
 ** Global Variables **
 **********************/
 char userInput; // Used for menu navigation
 

/********************
 ** Setup Function **
 ********************/

void setup(void) {
  setupFun(); // Defined in Z_Setup
}

/********************
 ** Loop Function **
 ********************/

void loop(void) {
  while (Serial.available() < 2){   // Wait for input from the User.
    }  
  userInput = Serial.read();        // Read user inputted variable
 
  while (Serial.available() > 0){   // Clear Serial Buffer
    Serial.read();
  }

  menuSelect(userInput);
}
