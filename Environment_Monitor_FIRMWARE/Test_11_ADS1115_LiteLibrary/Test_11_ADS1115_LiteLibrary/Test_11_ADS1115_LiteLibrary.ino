#include <ADS1115_lite.h>

ADS1115_lite adc(ADS1115_DEFAULT_ADDRESS); //Initializes wire library, sets private configuration variables to ADS1115 default(2.048V, 128SPS, Differential mode between  AIN0 and AIN1.  The Address parameter is not required if you want default
int16_t Raw;
//unsigned long starttime;
//unsigned long stoptime ;

const int N = 10;
int16_t Raw_array[N];  //an array capable of holding 5 entries numbered 0 to 4
unsigned long time_array[N];

void setup() {
  Serial.begin(57600);
  adc.setGain(ADS1115_REG_CONFIG_PGA_4_096V);
  adc.setSampleRate(ADS1115_REG_CONFIG_DR_860SPS); //Set the slowest and most accurate sample rate
  adc.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0); //Set single ended mode between AIN0 and GND
  Wire.setClock(400000);

}

void loop() {

  int i = 0;
  while (i < N) {
    //starttime = micros(); //Record a start time for demonstration
    adc.triggerConversion(); //Start a conversion.  This immediatly returns
    Raw = adc.getConversion(); //This polls the ADS1115 and wait for conversion
    //stoptime = micros(); //Record a start time for demonstration
    Raw_array[i] = Raw;
    //time_array[i] = stoptime - starttime;
    i = i + 1;
  }

  for (i = 0; i < N; i++) {
    Serial.println(Raw_array[i]);
    //Serial.print(" ");
    //Serial.println(time_array[i]);
  }

  delay(3000);

}
