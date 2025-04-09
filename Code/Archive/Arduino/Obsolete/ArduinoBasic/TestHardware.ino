/****************************************************************
  Author Name: Charlie Lambert and Curtis Williams
  Date: 3/8/2019
  Sketch Name: TestHardwareSketch
  Sketch Description: Tests the hardware of the cannon.

  Button Usage: Up/Down    -
              Left/Right -
              Select     -

  Pin Usage:    Pin type/number     Hardware
              ----------------    ----------------
              A0                   Horizontal Potentiometer
******************/


/***********************
 ** Global Variables ***
 ***********************/
#include <SimpleKalmanFilter.h>
SimpleKalmanFilter kfHorz(2, 2, 0.01);
SimpleKalmanFilter kfVert(2, 2, 0.01);
SimpleKalmanFilter kfBase(2, 2, 0.01);
SimpleKalmanFilter kfArm(2, 2, 0.01);

// *** Constants ***
double test = 3.14159265;
double twotest = 6.28318530;

// *** Declare & Initialize Pins ***
const int HORZPIN = A0;
const int VERTPIN = A1;
const int BASEPIN = A2;
const int ARMPIN = A3;

// *** Potentiometer Variables ***

int horzReading = 0;
double d3 = 0;
int vertReading = 0;
double d1 = 0;
int baseReading = 0;
double theta1 = 0;
int armReading = 0;
double theta4 = 0;


// *** Mapping Limits ***
double horzLLim = 10.0;
double horzULim = 80.0;
double vertLLim = 10.0;
double vertULim = 80.0;
double baseLLim = 0.0;
double baseULim = test;
double armLLim = 0.0;
double armULim = test;

// *** Misc ***
char userInput = 'z';
double HT[4][4];

/********************
 ** Setup Function **
 ********************/
void setup(void) {

  // *** Initialize Serial Communication ***
  Serial.begin(9600);

  // *** Take Initial Readings ***
  PrintMenu();


}// end setup() function

/*******************
 ** Loop Function **
 *******************/
void loop(void) {
  while (Serial.available() < 2); {
  }
  userInput = Serial.read(); //read character that user typed
  Serial.read();// reading newline character to empty buffer

  Serial.print("User input was ");
  Serial.println(userInput);
  delay(250);

  switch (userInput) {
    case 'a':
      Serial.println("Horizontal Translational Potentiometer Selected");
      while (Serial.available() == 0) {
        TestHorz();
      }
      break;

    case 'b':
      Serial.println("Vertical Translational Potentiometer Selected");
      while (Serial.available() == 0) {
        TestVert();
      }
      break;

    case 'c':
      Serial.println("Base Rotational Potentiometer Selected");
      while (Serial.available() == 0) {
        TestBase();
      }
      break;


    case 'd':
      Serial.println("Arm Rotational Potentiometer Selected");
      while (Serial.available() == 0) {
        TestArm();
      }
      break;

    case 'e':
      Serial.println("Forward Kinematics Selected");
      while (Serial.available() == 0) {
        FK();
      }
      break;


    case 'z': {
        PrintMenu();
      }
      break;

  } // end switch
} // end loop() function


/****************************
 ** User-Defined Functions **
 ****************************/

double mapping(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
