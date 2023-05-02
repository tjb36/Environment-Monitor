#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);


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

void setup() {
  Serial.begin(57600);
  ads.begin();
  ads.setGain(GAIN_ONE);
}

void loop() {

  int16_t adc;
  float avrg_cnt = 0.0;
  int i = 0;
  Serial.println("ADC Voltage:");
  while (i<5) {
    adc = ads.readADC_SingleEnded(2);
    //ads.readADC_Differential_0_1();
    //Serial.println(float(adc)*voltsPerBit,3);
    Serial.println(float(adc),0);
    avrg_cnt = avrg_cnt + float(adc);
    i = i + 1;
    delay(1);
  }
  Serial.print("Average: ");
  Serial.println(avrg_cnt/5.0 , 0);
  Serial.print("Average voltage: ");
  Serial.println(avrg_cnt/5.0*voltsPerBit , 3);
  Serial.println("");

  delay(3000);
}
