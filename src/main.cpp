#include <Arduino.h>
#include "DisplayDrv_st7789.h"
#include "globals.h"

// Create display instance
LGFX_ST7789 tft;

// MIDI CC Names (0-127)
const char *CC_NAMES[] = {
    "Bank Sel", "Mod Wheel", "Breath", "CC3", "Foot Ctrl", "Port Time", "Data MSB", "Volume",
    "Balance", "CC9", "Pan", "Express", "Effect 1", "Effect 2", "CC14", "CC15",
    "Gen Purp1", "Gen Purp2", "Gen Purp3", "Gen Purp4", "CC20", "CC21", "CC22", "CC23",
    "CC24", "CC25", "CC26", "CC27", "CC28", "CC29", "CC30", "CC31",
    "Bank LSB", "Mod Wheel", "Breath", "CC35", "Foot Ctrl", "Port Time", "Data LSB", "Volume",
    "Balance", "CC41", "Pan", "Express", "Effect 1", "Effect 2", "CC46", "CC47",
    "Gen Purp1", "Gen Purp2", "Gen Purp3", "Gen Purp4", "CC52", "CC53", "CC54", "CC55",
    "CC56", "CC57", "CC58", "CC59", "CC60", "CC61", "CC62", "CC63",
    "Sustain", "Portamen", "Sostenuto", "Soft Ped", "Legato", "Hold 2", "Sound 1", "Sound 2",
    "Sound 3", "Sound 4", "Sound 5", "Sound 6", "Sound 7", "Sound 8", "Sound 9", "Sound 10",
    "Gen Purp5", "Gen Purp6", "Gen Purp7", "Gen Purp8", "Port Ctrl", "CC85", "CC86", "CC87",
    "CC88", "CC89", "CC90", "Reverb", "Tremolo", "Chorus", "Detune", "Phaser",
    "Data Inc", "Data Dec", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "CC102", "CC103",
    "CC104", "CC105", "CC106", "CC107", "CC108", "CC109", "CC110", "CC111",
    "CC112", "CC113", "CC114", "CC115", "CC116", "CC117", "CC118", "CC119",
    "All Snd Off", "Reset Ctrl", "Local Ctrl", "All Nt Off", "Omni Off", "Omni On", "Mono On", "Poly On"};

// Note names (0-127)
const char *NOTE_NAMES[] = {
    "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
    "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
    "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
    "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
    "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"};

// MIDI message types
enum MidiMessageType
{
  MSG_CC = 0,  // Control Change
  MSG_PC = 1,  // Program Change
  MSG_NOTE = 2 // Note
};

// Current MIDI data
struct MidiData
{
  MidiMessageType type;
  uint8_t inNumber;  // CC number, PC number, or Note number
  uint8_t inValue;   // CC value, or Note velocity
  uint8_t outNumber; // Mapped CC/PC/Note number
  uint8_t outValue;  // Mapped value
};

MidiData currentMidi = {MSG_CC, 12, 123, 16, 40};

// LED indicator variables
unsigned long ledOnTime = 0;
const int LED_DURATION = 150; // LED stays on for 150ms
bool ledState = false;
const int LED_X = 16;      // LED position X (near right edge)
const int LED_Y = 16;        // LED position Y (near top)
const int LED_RADIUS = 5;   // LED circle radius

// Function to turn LED on (green circle)
void ledOn() {
  tft.fillCircle(LED_X, LED_Y, LED_RADIUS, TFT_GREEN);
  ledState = true;
  ledOnTime = millis();
}

// Function to turn LED off (dark circle)
void ledOff() {
  tft.fillCircle(LED_X, LED_Y, LED_RADIUS, TFT_DARKGREY);
  ledState = false;
}

// Function to update LED state (call in loop)
void updateLED() {
  if (ledState && (millis() - ledOnTime >= LED_DURATION)) {
    ledOff();
  }
}

// Function to get display name based on message type
String getMidiName(MidiMessageType type, uint8_t number)
{
  switch (type)
  {
  case MSG_CC:
    if (number <= 127)
    {
      return "CC" + String(number);
    }
    return "CC???";
  case MSG_PC:
    return "PC" + String(number);
  case MSG_NOTE:
    if (number <= 127)
    {
      return String(NOTE_NAMES[number]);
    }
    return "???";
  default:
    return "???";
  }
}

// Function to update the display with current MIDI data
void updateDisplay()
{
  // Clear previous values (draw black rectangles over old text)
  tft.fillRect(20, 56, 130, 35, TFT_BLACK);   // IN command area
  tft.fillRect(165, 56, 145, 35, TFT_BLACK);  // OUT command area
  tft.fillRect(20, 105, 130, 35, TFT_BLACK);  // IN value area
  tft.fillRect(165, 105, 145, 35, TFT_BLACK); // OUT value area

  // Display IN MIDI command
  tft.setTextSize(4);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);  // Set text color with black background
  tft.setCursor(20, 56);
  tft.print(getMidiName(currentMidi.type, currentMidi.inNumber));

  // Display OUT MIDI command
  tft.setCursor(180, 56);
  tft.print(getMidiName(currentMidi.type, currentMidi.outNumber));

  // Display values (only for CC and Notes, not for PC)
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);  // Set text color with black background
  if (currentMidi.type != MSG_PC)
  {
    tft.setCursor(20, 105);
    tft.print(currentMidi.inValue);
    tft.setCursor(180, 105);
    tft.print(currentMidi.outValue);
  }

  tft.drawFastVLine(159, 2, 168, TFT_DARKGREY);
  
  // Trigger LED blink on every update
  ledOn();
}

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
  tft.setCursor(70, 10);
  tft.println("IN");

  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREEN);
  tft.setCursor(220, 10);
  tft.println("OUT");

  // tft.drawTriangle(150, 8, 150, 28, 160, 18, TFT_DARKGREY); // on top
  // tft.drawFastHLine(3, 34, 314, TFT_DARKGREY);
  tft.drawFastVLine(159, 2, 160, TFT_DARKGREY);

  // Draw border with rounded corners
  tft.drawRoundRect(0, 0, 320, 172, 23, TFT_MAGENTA);
  
  // Initialize LED indicator in off state
  ledOff();

  // Initial display update
  updateDisplay();

  Serial.println("Display initialized with MIDI data!");
}

void loop()
{
  // Update LED state (turn off after duration)
  updateLED();
  
  // Demo: Cycle through different MIDI message types every 3 seconds
  static unsigned long lastUpdate = 0;
  static uint8_t demoMode = 0;

  if (millis() - lastUpdate > 1000)
  {
    lastUpdate = millis();

    // Cycle through different demo scenarios
    switch (demoMode)
    {
    case 0: // CC example
      currentMidi.type = MSG_CC;
      currentMidi.inNumber = 12;
      currentMidi.inValue = random(0, 128);
      currentMidi.outNumber = 16;
      currentMidi.outValue = random(0, 128);
      Serial.println("Demo: CC12 -> CC16");
      break;

    case 1: // Different CC
      currentMidi.type = MSG_CC;
      currentMidi.inNumber = 74;
      currentMidi.inValue = random(0, 128);
      currentMidi.outNumber = 71;
      currentMidi.outValue = random(0, 128);
      Serial.println("Demo: CC74 -> CC71");
      break;

    case 2: // Program Change
      currentMidi.type = MSG_PC;
      currentMidi.inNumber = random(0, 128);
      currentMidi.outNumber = random(0, 128);
      Serial.println("Demo: PC -> PC");
      break;

    case 3: // Note example
      currentMidi.type = MSG_NOTE;
      currentMidi.inNumber = 60; // Middle C
      currentMidi.inValue = random(60, 127);
      currentMidi.outNumber = 64; // E
      currentMidi.outValue = random(60, 127);
      Serial.println("Demo: Note C4 -> E4");
      break;

    case 4: // Another note
      currentMidi.type = MSG_NOTE;
      currentMidi.inNumber = 72; // C5
      currentMidi.inValue = random(60, 127);
      currentMidi.outNumber = 76; // E5
      currentMidi.outValue = random(60, 127);
      Serial.println("Demo: Note C5 -> E5");
      break;
    }

    // Update display with new data
    updateDisplay();

    // Cycle to next demo mode
    demoMode = (demoMode + 1) % 5;
  }
}