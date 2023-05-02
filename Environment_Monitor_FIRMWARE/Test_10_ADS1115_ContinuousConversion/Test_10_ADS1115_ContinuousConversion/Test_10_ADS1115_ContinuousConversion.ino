#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

constexpr int READY_PIN = 3; // Pin connected to the ALERT/RDY signal for new sample notification.
volatile bool new_data = false;


// Default gain setting for ADS1115 is 2/3 , corresponding to a full-scale range of +/- 6.144 V (see datasheet).
// When using single-ended mode, you can only get positive measurements with respect to GND, and so the "sign"
// bit is not used - this reduces the 16 bit ADC to effectively 15 bit (2^15 = 32768). Therefore, 1 bit corresponds
// to 6.144 / 32768 = 0.1875 mV, and so 5 V = 26667 counts. Using a gain setting of 2/3 allows the measurement to go
// all the way up to the supply voltage of VDD = 5 V without saturating the ADC.
// NOTE, regardless, you should still never put more than 0.3 V above VDD into the ADC's input, as per the datasheet!

//                                                                ADS1015  ADS1115
//                                                                -------  -------
// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

//float voltsPerBit = 0.0001875;  // Conversion from ADC counts into voltage [V].
float voltsPerBit = 0.000125;  // Conversion from ADC counts into voltage [V].
float voltage;

const int N = 10;
int BX_array[N];  //an array capable of holding 5 entries numbered 0 to 4

void setup() {
  Serial.begin(57600);
  ads.begin(0x48);
  ads.setGain(GAIN_ONE);
  ads.setDataRate(RATE_ADS1115_860SPS);
  Wire.setClock(400000);

  pinMode(READY_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(READY_PIN), NewDataReadyISR, FALLING);  // We get a falling edge every time a new sample is ready.

  // Start continuous conversions.
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, /*continuous=*/true);

//  Serial.println("Beginning Main Loop");
//  int i = 0;
//  long t1 = micros();
//  while (i < N) {
//    //Serial.println("Start of loop");
//    //Serial.println(new_data);
//    while (!new_data) {
//      //Serial.println(new_data);
//      }
//    //Serial.println("");
//    int16_t results = ads.getLastConversionResults();
//    BX_array[i] = results;
//    i = i+1;
//    new_data = false;
//  }
//  long t2 = micros();
//  Serial.println("Finished!");
//  Serial.println(t2 - t1);
//  for (i = 0; i < N; i++) {
//    Serial.println(BX_array[i]);
//  }



}

void loop() {

  //  int16_t adc;
  //  int i = 0;
  //  long t1 = millis();
  //
  //  while (i<5) {
  //    adc = ads.readADC_SingleEnded(0);
  //    BX_array[i] = adc;
  //    Serial.println(float(adc),0);
  //    i = i + 1;
  //  }
  //  long t2 = millis();
  //  Serial.print("Time: ");
  //  Serial.println(t2-t1);
  //
  //
  //
  //  delay(3000);
//  delay(3000);
//  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, /*continuous=*/false);
//  //int16_t adc;
//  //adc = ads.readADC_SingleEnded(0);
//  //Serial.println(adc);
//  delay(10000);
}

void IRAM_ATTR NewDataReadyISR() {
  new_data = true;
}
