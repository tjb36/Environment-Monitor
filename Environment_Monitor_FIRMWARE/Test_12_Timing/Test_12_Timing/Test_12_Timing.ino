
// Variables for ensuring data is collected at regular time intervals
const unsigned long samplingInterval = 5000;    // How often to take a data sample in ms (must be >25ms, since this is time needed to update OLED screen in loop)
unsigned long previousSampledMillis = 0;      // Time at which previous sample was acquired
unsigned long currentMillis = 0;              // Current time elapsed since program started in ms

void setup() {
  Serial.begin(57600);
  delay(6000);
}

void loop() {


  currentMillis = millis();

  // When sampling interval has elapsed, go into this loop and take data points ////////////////////////////////////
  if ((currentMillis - previousSampledMillis) >= samplingInterval) {

    Serial.print("currentMillis: ");
    Serial.println(currentMillis);
    Serial.print("previousSampledMillis: ");
    Serial.println(previousSampledMillis);
    Serial.println("");
    
    previousSampledMillis += samplingInterval; // Keep track of last time that a sample was taken
  }

}
