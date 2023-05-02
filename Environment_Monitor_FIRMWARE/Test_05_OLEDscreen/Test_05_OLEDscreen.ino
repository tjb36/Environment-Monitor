#include <U8x8lib.h>
#include <Wire.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

void setup(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);
}

void loop(void)
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  int t1 = millis();
  u8x8.drawString(0, 0, "H");
  int t2 = millis();

  delay(2000);
  u8x8.drawString(5, 0, "G");
  delay(2000);

}
