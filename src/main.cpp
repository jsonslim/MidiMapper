#include <Arduino.h>
#include "DisplayDrv_st7789.h"
#include "globals.h"

// Create display instance
LGFX_ST7789 tft;

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32-C3 ST7789 Display Test");

  // Initialize Serial1 for MIDI (optional, for future use)
  Serial1.begin(31250, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);
  Serial.println("MIDI Serial1 initialized on TX:GPIO6, RX:GPIO7");

  // Initialize display
  tft.init();
  Serial.println("Display initialized");

  // Set rotation (0-3)
  tft.setRotation(1); // Landscape mode

  // Clear screen with black
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // tft.setCursor(30, 100);
  // tft.println("Hello World!");

  // Add some colorful text
  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREEN);
  tft.setCursor(20, 10);
  tft.println("IN");

  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREEN);
  tft.setCursor(190, 10);
  tft.println("OUT");

  // tft.drawTriangle(150, 8, 150, 28, 160, 18, TFT_DARKGREY); // on top
  // tft.drawFastHLine(3, 34, 314, TFT_DARKGREY);

  tft.setTextSize(4);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(20, 56);
  tft.println("CC12");

  tft.setCursor(190, 56);
  tft.println("CC16");

  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(20, 105);
  tft.println("123");
  tft.setCursor(190, 105);
  tft.println("40");

  // Draw some shapes with rounded corners
  // Draw a border with rounded corners (inset from edges to see the rounding)
  tft.drawRoundRect(0, 0, 320, 172, 23, TFT_MAGENTA);
  // tft.drawFastVLine(160, 40, 110, TFT_DARKGREY);

  // tft.drawTriangle(148, 60, 148, 140, 170, 100, TFT_DARKGREY); // on the middle

  // tft.drawLine(160, 40, 170, 90, TFT_DARKGREY);
  // tft.drawLine(160, 150, 170, 90, TFT_DARKGREY);

  Serial.println("Hello World displayed!");
}

void loop()
{
  // Blinking text effect
  static unsigned long lastUpdate = 0;
  static bool toggle = false;

  if (millis() - lastUpdate > 1000)
  {
    lastUpdate = millis();
    toggle = !toggle;

    tft.setTextSize(1);
    tft.setCursor(120, 300);
    if (toggle)
    {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println("* READY *");
    }
    else
    {
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      tft.println("* READY *");
    }
  }
}