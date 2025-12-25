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
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(70, 10);
  tft.println("IN");

  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(230, 10);
  tft.println("OUT");

  tft.setTextSize(4);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(30, 80);
  tft.println("CC12");

  tft.setCursor(190, 80);
  tft.println("CC16");

  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(40, 210);
  tft.println("LovyanGFX");

  // Draw some shapes with rounded corners
  // Draw a border with rounded corners (inset from edges to see the rounding)
  tft.drawRoundRect(0, 0, 320, 172, 23, TFT_MAGENTA);
  tft.drawFastVLine(160, 10, 150, TFT_DARKGREY);

  // Test: Draw a filled rounded rectangle to verify it works
  // tft.fillRoundRect(250, 130, 60, 30, 8, TFT_DARKGREEN);
  // tft.setTextSize(1);
  // tft.setTextColor(TFT_WHITE);
  // tft.setCursor(260, 140);
  // tft.print("TEST");

  // tft.drawCircle(160, 260, 30, TFT_RED);
  // tft.fillCircle(160, 260, 25, TFT_BLUE);

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