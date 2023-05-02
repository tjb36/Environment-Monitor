#include <Adafruit_MAX31865.h>
#include <SPI.h>    // Needed for SD card
#include <Wire.h>

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(4, 5, 6, 7);
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(9);
float T_PT;

// The value of the Rref resistor. Use 430.0 for PT100
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor, 100.0 for PT100
#define RNOMINAL  100.0

void setup() {
  Serial.begin(57600);
  delay(1000);
  thermo.begin(MAX31865_4WIRE);  // initialize the sensor

}

void loop() {
  long t1 = millis();
  T_PT = thermo.temperature(RNOMINAL, RREF);
  long t2 = millis();
  Serial.println(T_PT);
  Serial.println(t2-t1);
  
  delay(1000);
}
