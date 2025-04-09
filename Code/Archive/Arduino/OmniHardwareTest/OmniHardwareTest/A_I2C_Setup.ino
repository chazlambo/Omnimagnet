/****************************
 ** #defines and #includes **
 ****************************/
#define TCAADDR 0x70              // Address of I2C Multiplexer
#define ADC_REFERENCE_VOLTAGE 5.0 // System runs off of 5 volts.




/***********************
 ** Global Variables ***
 ***********************/
// Initialize ADC Modules
const int ADC_ADDRESS[5] = {0x48, 0x49, 0x4C, 0x4D, 0x4F};
Adafruit_PCF8591 arm1_pose = Adafruit_PCF8591();
Adafruit_PCF8591 arm2_pose = Adafruit_PCF8591();
Adafruit_PCF8591 arm3_pose = Adafruit_PCF8591();
Adafruit_PCF8591 arm4_pose = Adafruit_PCF8591();
Adafruit_PCF8591 arm5_pose = Adafruit_PCF8591();

Adafruit_PCF8591 arm1_temp = Adafruit_PCF8591();
Adafruit_PCF8591 arm2_temp = Adafruit_PCF8591();
Adafruit_PCF8591 arm3_temp = Adafruit_PCF8591();
Adafruit_PCF8591 arm4_temp = Adafruit_PCF8591();
Adafruit_PCF8591 arm5_temp = Adafruit_PCF8591();

/***********************
 ** User Functions ***
 ***********************/

// I2C Setup Function
void setup_I2C(void) {
  // Initialize I2C Bus
  Wire.begin();

  // Initialize Temperature ADC Modules
  arm1_temp.begin(ADC_ADDRESS[0]);
  arm2_temp.begin(ADC_ADDRESS[1]);
  arm3_temp.begin(ADC_ADDRESS[2]);
  arm4_temp.begin(ADC_ADDRESS[3]);
  arm5_temp.begin(ADC_ADDRESS[4]);


  // Initialize Position ADC Modules
  arm1_pose.begin(ADC_ADDRESS[0]);
  arm2_pose.begin(ADC_ADDRESS[1]);
  arm3_pose.begin(ADC_ADDRESS[2]);
  arm4_pose.begin(ADC_ADDRESS[3]);
  arm5_pose.begin(ADC_ADDRESS[4]);


  // Enable DAC For ADC Modules
  //  This is to enable communication to the the LEDs on top of each arm
  //  This is only for the position ADC modules
  arm1_pose.enableDAC(true);
  arm2_pose.enableDAC(true);
  arm3_pose.enableDAC(true);
  arm4_pose.enableDAC(true);
  arm5_pose.enableDAC(true);

}

// Multiplexer Port Select Function
void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Arm Communication
void armCommStart(int arm) {
  tcaselect(1);
  switch (arm) {
    case 1:
      arm1_pose.analogWrite(255);
      break;

    case 2:
      arm2_pose.analogWrite(255);
      break;

    case 3:
      arm3_pose.analogWrite(255);
      break;

    case 4:
      arm4_pose.analogWrite(255);
      break;

    case 5:
      arm5_pose.analogWrite(255);
      break;
  }
}

void armCommEnd(int arm) {
  tcaselect(1);
  switch (arm) {
    case 1:
      arm1_pose.analogWrite(0);
      break;

    case 2:
      arm2_pose.analogWrite(0);
      break;

    case 3:
      arm3_pose.analogWrite(0);
      break;

    case 4:
      arm4_pose.analogWrite(0);
      break;

    case 5:
      arm5_pose.analogWrite(0);
      break;
  }
}

// ADC Print Function
int adcPrint(Adafruit_PCF8591 &pcf, int bus, int arm) {
  tcaselect(bus); // Open I2C line for desired bus

  // Check if arm is connected
  if (!pcf.begin(ADC_ADDRESS[arm - 1])) {
    Serial.print("ERROR: ARM ADC NOT FOUND FOR ARM #"); Serial.println(arm);
    return;
  }
    Serial.print("\t"); Serial.print(pcf.analogRead(0)); // Print value from A0
    Serial.print("\t"); Serial.print(pcf.analogRead(1)); // Print value from A1
    Serial.print("\t"); Serial.print(pcf.analogRead(2)); // Print value from A2
    Serial.print("\t"); Serial.println(pcf.analogRead(3)); // Print value from A3
}
