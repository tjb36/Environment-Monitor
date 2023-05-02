#include <U8x8lib.h>
#include <Wire.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

unsigned long currentMillis = 0;              // Current time elapsed since program started in ms
unsigned long currentRelativeMillis = 0;      // Relative time after resetting the zeroRelativeMillis variable
unsigned long zeroRelativeMillis = 0;         // Zero of relative time
const unsigned long samplingInterval = 1000;    // How often to take a data sample in ms (must be >25ms, since this is time needed to update OLED screen in loop)
unsigned long previousSampledMillis = 0;      // Time at which previous sample was acquired
const unsigned long screenUpdateInterval = 100;
unsigned long previousScreenUpdateMillis = 0;
char result1[7]; // Preallocate character arrays for text to appear on screen
char result2[7];
char result3[7];
char result4[7];
float BX = 50.000;
float BY = 50.000;
float BZ = 50.000;
float T = 25.12;

void setup(void)
{
  // Start OLED display screen, and display a welcome message /////
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  u8x8.drawString(4, 0, "Welcome!");
  u8x8.drawString(2, 2, "Environment");
  u8x8.drawString(4, 3, "Monitor");
  u8x8.drawString(2, 6, "Starting...");
  /////////////////////////////////////////////////////////////////

  delay(3000);

  // Initialise parameters on the OLED display screen /////////////
  u8x8.clearDisplay();
  //u8x8.drawString(0, 0, "Lock Disabled");
  //u8x8.drawString(0, 1, "PID Within Lims ");
  u8x8.drawString(0, 2, "BX = ");
  u8x8.drawString(5, 2, "50.000");
  u8x8.drawString(13, 2, "uT");
  u8x8.drawString(0, 3, "BY = ");
  u8x8.drawString(5, 3, "55.000");
  u8x8.drawString(13, 3, "uT");
  u8x8.drawString(0, 4, "BZ = ");
  u8x8.drawString(5, 4, "58.000");
  u8x8.drawString(13, 4, "uT");
  u8x8.drawString(0, 5, "T  = ");
  u8x8.drawString(5, 5, "26.15");
  u8x8.drawString(12, 5, "degC");
  /////////////////////////////////////////////////////////////////
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop(void)
{

  currentMillis = millis();
  currentRelativeMillis = currentMillis - zeroRelativeMillis;

  // When sampling interval has elapsed, go into this loop, take data points, and update PID ////////////////////////////////////
  if ((currentRelativeMillis - previousSampledMillis) >= samplingInterval) {
    BX -= 0.1;
    T += 0.1;
    BX = bound(BX, -70, 70);
    BY = bound(BY, -70, 70);
    BZ = bound(BZ, -70, 70);
    previousSampledMillis += samplingInterval; // Keep track of time
  }

  if ((currentRelativeMillis - previousScreenUpdateMillis) >= screenUpdateInterval) {

    // Convert floats to character arrays, so that they can be printed to screen
    dtostrf(BX, 5, 3, result1);
    dtostrf(BY, 5, 3, result2);
    dtostrf(BZ, 5, 3, result3);
    dtostrf(T,  5, 2, result4);

    if (BX >= 0) {
      u8x8.drawString(5, 2, "+");
      u8x8.drawString(6, 2, result1);
    }
    else {
      u8x8.drawString(5, 2, result1);
    }

    if (BY >= 0) {
      u8x8.drawString(5, 3, "+");
      u8x8.drawString(6, 3, result2);
    }
    else {
      u8x8.drawString(5, 3, result2);
    }

    if (BZ >= 0) {
      u8x8.drawString(5, 4, "+");
      u8x8.drawString(6, 4, result3);
    }
    else {
      u8x8.drawString(5, 4, result3);
    }

    u8x8.drawString(5, 5, result4);

    previousScreenUpdateMillis += screenUpdateInterval;
  }
  delay(10);

}

// A function just to make sure output stays within some limits //////////////////////////////////////////////////////////////////
float bound(float x, float x_min, float x_max) {
  if (x < x_min) {
    x = x_min;
  }
  if (x > x_max) {
    x = x_max;
  }
  return x;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
