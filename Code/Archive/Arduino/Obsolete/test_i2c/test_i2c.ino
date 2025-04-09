#include <Adafruit_PCF8591.h>
// Make sure that this is set to the value in volts of VCC
#define ADC_REFERENCE_VOLTAGE 5.0
Adafruit_PCF8591 arm1 = Adafruit_PCF8591();
Adafruit_PCF8591 arm2 = Adafruit_PCF8591();
Adafruit_PCF8591 arm3 = Adafruit_PCF8591();
Adafruit_PCF8591 arm4 = Adafruit_PCF8591();
Adafruit_PCF8591 arm5 = Adafruit_PCF8591();


void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  arm1.begin(0x48);
  arm2.begin(0x49);
  arm3.begin(0x4C);
  arm4.begin(0x4E);
  arm5.begin(0x4F);

  Serial.println("# Adafruit PCF8591 demo");
  if (!arm1.begin()) {
    Serial.println("# ARM 1 ADC NOT FOUND ");
    while (1)
      delay(10);
  }
  if (!arm1.begin()) {
    Serial.println("# ARM 2 ADC NOT FOUND ");
    while (1)
      delay(10);
  }
  if (!arm1.begin()) {
    Serial.println("# ARM 3 ADC NOT FOUND ");
    while (1)
      delay(10);
  }
  if (!arm1.begin()) {
    Serial.println("# ARM 4 ADC NOT FOUND ");
    while (1)
      delay(10);
  }
  if (!arm1.begin()) {
    Serial.println("# ARM 5 ADC NOT FOUND ");
    while (1)
      delay(10);
  }
  
  Serial.println("# Adafruit PCF8591 found");
  arm1.enableDAC(true);

  // Serial.println("AIN0, AIN1, AIN2, AIN3");
  Serial.println();
  Serial.print("Arm 3:");
  Serial.print("\t");
  Serial.print("Wrist");
  Serial.print("\t");
  Serial.print("Horz.");
  Serial.print("\t");
  Serial.print("Base");
  Serial.print("\t");
  Serial.println("Vert.");
}

uint8_t dac_counter = 0;

void loop() {
  Serial.print("\t");
  Serial.print(arm1.analogRead(1));
  Serial.print("\t");
  Serial.print(arm2.analogRead(1));
  Serial.print("\t");
  Serial.print(arm3.analogRead(1));
  Serial.print("\t");
  Serial.print(arm4.analogRead(1));
  Serial.print("\t");
  Serial.println(arm5.analogRead(1));
  delay(50);
}
