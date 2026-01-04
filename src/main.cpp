#include <Arduino.h>
#include <ArduinoJson.h>
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

// Demo mode control
bool demoEnabled = false; // Set to false to disable demo and use serial commands only

// LED indicator variables
unsigned long ledOnTime = 0;
const int LED_DURATION = 150; // LED stays on for 150ms
bool ledState = false;
const int LED_X = 16;     // LED position X (near right edge)
const int LED_Y = 16;     // LED position Y (near top)
const int LED_RADIUS = 5; // LED circle radius

// JSON Mapping structure
JsonDocument mapDoc;
bool mappingEnabled = true;

// Default mapping JSON (example with type conversion)
const char *defaultMapping = R"({
  "cc_map": {
    "12": 16,
    "23": "note:45",
    "74": [71, 72, "note:60"],
    "1": {"type": "note", "num": 64, "scale": 1.0},
    "7": {"num": 77, "scale": 0.8}
  },
  "pc_map": {
    "0": 10,
    "5": ["cc:74", "note:60"],
    "10": {"type": "cc", "num": 100}
  },
  "note_map": {
    "60": 64,
    "62": ["cc:74", "note:67"],
    "72": {"type": "cc", "num": 76, "velocity": 1.2}
  }
})";

// Output structure for multiple mappings
struct MappedOutput
{
  MidiMessageType type; // Output message type (can be different from input)
  uint8_t number;
  uint8_t value;
};

// Function to apply value scaling/transformation
uint8_t scaleValue(uint8_t inputValue, float scale)
{
  int scaled = (int)(inputValue * scale);
  if (scaled > 127)
    scaled = 127;
  if (scaled < 0)
    scaled = 0;
  return (uint8_t)scaled;
}

// Function to get mapping key based on message type
String getMappingKey(MidiMessageType type)
{
  switch (type)
  {
  case MSG_CC:
    return "cc_map";
  case MSG_PC:
    return "pc_map";
  case MSG_NOTE:
    return "note_map";
  default:
    return "";
  }
}

// Function to apply mapping
// Returns true if mapping was applied, false if pass-through
bool applyMapping(MidiData &midi, MappedOutput outputs[], int &outputCount)
{
  outputCount = 0;

  if (!mappingEnabled)
  {
    // Pass-through mode
    outputs[0].type = midi.type;
    outputs[0].number = midi.inNumber;
    outputs[0].value = midi.inValue;
    outputCount = 1;
    return false;
  }

  String mapKey = getMappingKey(midi.type);
  String inKey = String(midi.inNumber);

  if (!mapDoc.containsKey(mapKey))
  {
    // No mapping for this type, pass through
    outputs[0].type = midi.type;
    outputs[0].number = midi.inNumber;
    outputs[0].value = midi.inValue;
    outputCount = 1;
    return false;
  }

  JsonObject map = mapDoc[mapKey];

  if (!map.containsKey(inKey))
  {
    // No mapping for this specific number, pass through
    outputs[0].type = midi.type;
    outputs[0].number = midi.inNumber;
    outputs[0].value = midi.inValue;
    outputCount = 1;
    return false;
  }

  JsonVariant mapping = map[inKey];

  // Handle different mapping types
  if (mapping.is<int>())
  {
    // Simple number mapping: "12": 16 (same type)
    outputs[0].type = midi.type;
    outputs[0].number = mapping.as<int>();
    outputs[0].value = midi.inValue;
    outputCount = 1;
  }
  else if (mapping.is<String>())
  {
    // String mapping for type conversion: "23": "note:45"
    String mapStr = mapping.as<String>();
    int colonPos = mapStr.indexOf(':');

    if (colonPos > 0)
    {
      String typeStr = mapStr.substring(0, colonPos);
      int targetNum = mapStr.substring(colonPos + 1).toInt();

      // Determine output type
      typeStr.toLowerCase();
      if (typeStr == "cc")
      {
        outputs[0].type = MSG_CC;
      }
      else if (typeStr == "pc")
      {
        outputs[0].type = MSG_PC;
      }
      else if (typeStr == "note" || typeStr == "nn")
      {
        outputs[0].type = MSG_NOTE;
      }
      else
      {
        outputs[0].type = midi.type; // Unknown, keep same type
      }

      outputs[0].number = targetNum;
      outputs[0].value = midi.inValue;
      outputCount = 1;
    }
    else
    {
      // No colon, treat as number
      outputs[0].type = midi.type;
      outputs[0].number = mapStr.toInt();
      outputs[0].value = midi.inValue;
      outputCount = 1;
    }
  }
  else if (mapping.is<JsonArray>())
  {
    // Array mapping (one-to-many): "12": [16, 17, "note:60"]
    JsonArray arr = mapping.as<JsonArray>();
    outputCount = 0;
    for (JsonVariant v : arr)
    {
      if (outputCount >= 10)
        break; // Max 10 outputs

      if (v.is<int>())
      {
        // Simple number (same type)
        outputs[outputCount].type = midi.type;
        outputs[outputCount].number = v.as<int>();
        outputs[outputCount].value = midi.inValue;
        outputCount++;
      }
      else if (v.is<String>())
      {
        // String with type conversion
        String mapStr = v.as<String>();
        int colonPos = mapStr.indexOf(':');

        if (colonPos > 0)
        {
          String typeStr = mapStr.substring(0, colonPos);
          int targetNum = mapStr.substring(colonPos + 1).toInt();

          typeStr.toLowerCase();
          if (typeStr == "cc")
          {
            outputs[outputCount].type = MSG_CC;
          }
          else if (typeStr == "pc")
          {
            outputs[outputCount].type = MSG_PC;
          }
          else if (typeStr == "note" || typeStr == "nn")
          {
            outputs[outputCount].type = MSG_NOTE;
          }
          else
          {
            outputs[outputCount].type = midi.type;
          }

          outputs[outputCount].number = targetNum;
          outputs[outputCount].value = midi.inValue;
          outputCount++;
        }
      }
    }
  }
  else if (mapping.is<JsonObject>())
  {
    // Object mapping with transformations: "12": {"type": "note", "num": 60, "scale": 0.5}
    JsonObject obj = mapping.as<JsonObject>();

    // Check for type conversion
    if (obj.containsKey("type"))
    {
      String typeStr = obj["type"].as<String>();
      typeStr.toLowerCase();
      if (typeStr == "cc")
      {
        outputs[0].type = MSG_CC;
      }
      else if (typeStr == "pc")
      {
        outputs[0].type = MSG_PC;
      }
      else if (typeStr == "note" || typeStr == "nn")
      {
        outputs[0].type = MSG_NOTE;
      }
      else
      {
        outputs[0].type = midi.type;
      }
    }
    else
    {
      outputs[0].type = midi.type; // Keep same type if not specified
    }

    outputs[0].number = obj["num"] | midi.inNumber; // Default to input if not specified

    if (obj.containsKey("scale"))
    {
      float scale = obj["scale"];
      outputs[0].value = scaleValue(midi.inValue, scale);
    }
    else if (obj.containsKey("velocity"))
    {
      float scale = obj["velocity"];
      outputs[0].value = scaleValue(midi.inValue, scale);
    }
    else
    {
      outputs[0].value = midi.inValue;
    }
    outputCount = 1;
  }

  return true;
}

// Function to load JSON mapping
void loadMapping(const char *jsonString)
{
  DeserializationError error = deserializeJson(mapDoc, jsonString);

  if (error)
  {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    mappingEnabled = false;
    return;
  }

  Serial.println("✓ Mapping loaded successfully");
  mappingEnabled = true;

  // Print loaded mappings
  Serial.println("\n=== Current Mappings ===");
  if (mapDoc.containsKey("cc_map"))
  {
    Serial.println("CC Mappings:");
    JsonObject ccMap = mapDoc["cc_map"];
    for (JsonPair kv : ccMap)
    {
      Serial.print("  CC");
      Serial.print(kv.key().c_str());
      Serial.print(" -> ");
      serializeJson(kv.value(), Serial);
      Serial.println();
    }
  }
  if (mapDoc.containsKey("pc_map"))
  {
    Serial.println("PC Mappings:");
    JsonObject pcMap = mapDoc["pc_map"];
    for (JsonPair kv : pcMap)
    {
      Serial.print("  PC");
      Serial.print(kv.key().c_str());
      Serial.print(" -> ");
      serializeJson(kv.value(), Serial);
      Serial.println();
    }
  }
  if (mapDoc.containsKey("note_map"))
  {
    Serial.println("Note Mappings:");
    JsonObject noteMap = mapDoc["note_map"];
    for (JsonPair kv : noteMap)
    {
      Serial.print("  Note ");
      Serial.print(kv.key().c_str());
      Serial.print(" -> ");
      serializeJson(kv.value(), Serial);
      Serial.println();
    }
  }
  Serial.println("=======================\n");
}

// Function to turn LED on (green circle)
void ledOn()
{
  tft.fillCircle(LED_X, LED_Y, LED_RADIUS, TFT_GREEN);
  ledState = true;
  ledOnTime = millis();
}

// Function to turn LED off (dark circle)
void ledOff()
{
  tft.fillCircle(LED_X, LED_Y, LED_RADIUS, TFT_DARKGREY);
  ledState = false;
}

// Function to update LED state (call in loop)
void updateLED()
{
  if (ledState && (millis() - ledOnTime >= LED_DURATION))
  {
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
  tft.setTextColor(TFT_CYAN, TFT_BLACK); // Set text color with black background
  tft.setCursor(20, 56);
  tft.print(getMidiName(currentMidi.type, currentMidi.inNumber));

  // Display OUT MIDI command
  tft.setCursor(180, 56);
  tft.print(getMidiName(currentMidi.type, currentMidi.outNumber));

  // Display values (only for CC and Notes, not for PC)
  tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Set text color with black background
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

  // Load default mapping
  loadMapping(defaultMapping);

  Serial.println("Display initialized with MIDI data!");
  Serial.println("\n=== MIDI Mapper Ready ===");
  Serial.println("Type 'help' for commands");
  Serial.println("Demo mode: " + String(demoEnabled ? "ENABLED" : "DISABLED"));
  Serial.println("========================\n");
}

// Serial command parser
// Format: cc_<number>_<value> or pc_<number> or nn_<number>_<velocity>
// Examples: cc_12_64, pc_5, nn_60_100
void parseSerialCommand()
{
  if (Serial.available() > 0)
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd.length() == 0)
      return;

    Serial.print("Received command: ");
    Serial.println(cmd);

    // Parse command type
    if (cmd.startsWith("cc_"))
    {
      // Control Change: cc_<number>_<value>
      int firstUnderscore = cmd.indexOf('_');
      int secondUnderscore = cmd.indexOf('_', firstUnderscore + 1);

      if (secondUnderscore > 0)
      {
        String numStr = cmd.substring(firstUnderscore + 1, secondUnderscore);
        String valStr = cmd.substring(secondUnderscore + 1);

        int ccNum = numStr.toInt();
        int ccVal = valStr.toInt();

        if (ccNum >= 0 && ccNum <= 127 && ccVal >= 0 && ccVal <= 127)
        {
          currentMidi.type = MSG_CC;
          currentMidi.inNumber = ccNum;
          currentMidi.inValue = ccVal;

          // Apply mapping
          MappedOutput outputs[10];
          int outputCount = 0;
          applyMapping(currentMidi, outputs, outputCount);

          // Display first output (for multi-output, show the first one)
          if (outputCount > 0)
          {
            currentMidi.type = outputs[0].type; // Update type in case it changed
            currentMidi.outNumber = outputs[0].number;
            currentMidi.outValue = outputs[0].value;
          }
          else
          {
            currentMidi.outNumber = ccNum;
            currentMidi.outValue = ccVal;
          }

          updateDisplay();
          Serial.printf("✓ CC%d Value:%d -> ", ccNum, ccVal);

          // Print all outputs with their types
          for (int i = 0; i < outputCount; i++)
          {
            switch (outputs[i].type)
            {
            case MSG_CC:
              Serial.printf("CC%d:%d", outputs[i].number, outputs[i].value);
              break;
            case MSG_PC:
              Serial.printf("PC%d", outputs[i].number);
              break;
            case MSG_NOTE:
              Serial.printf("Note %s:%d", NOTE_NAMES[outputs[i].number], outputs[i].value);
              break;
            }
            if (i < outputCount - 1)
              Serial.print(", ");
          }
          Serial.println();
        }
        else
        {
          Serial.println("✗ Error: CC number and value must be 0-127");
        }
      }
      else
      {
        Serial.println("✗ Error: Format should be cc_<number>_<value>");
      }
    }
    else if (cmd.startsWith("pc_"))
    {
      // Program Change: pc_<number>
      int firstUnderscore = cmd.indexOf('_');
      String numStr = cmd.substring(firstUnderscore + 1);

      int pcNum = numStr.toInt();

      if (pcNum >= 0 && pcNum <= 127)
      {
        currentMidi.type = MSG_PC;
        currentMidi.inNumber = pcNum;
        currentMidi.inValue = 0;

        // Apply mapping
        MappedOutput outputs[10];
        int outputCount = 0;
        applyMapping(currentMidi, outputs, outputCount);

        // Display first output
        if (outputCount > 0)
        {
          currentMidi.type = outputs[0].type; // Update type in case it changed
          currentMidi.outNumber = outputs[0].number;
          currentMidi.outValue = outputs[0].value;
        }
        else
        {
          currentMidi.outNumber = pcNum;
        }

        updateDisplay();
        Serial.printf("✓ PC%d -> ", pcNum);

        // Print all outputs with their types
        for (int i = 0; i < outputCount; i++)
        {
          switch (outputs[i].type)
          {
          case MSG_CC:
            Serial.printf("CC%d:%d", outputs[i].number, outputs[i].value);
            break;
          case MSG_PC:
            Serial.printf("PC%d", outputs[i].number);
            break;
          case MSG_NOTE:
            Serial.printf("Note %s:%d", NOTE_NAMES[outputs[i].number], outputs[i].value);
            break;
          }
          if (i < outputCount - 1)
            Serial.print(", ");
        }
        Serial.println();
      }
      else
      {
        Serial.println("✗ Error: PC number must be 0-127");
      }
    }
    else if (cmd.startsWith("nn_"))
    {
      // Note: nn_<number>_<velocity>
      int firstUnderscore = cmd.indexOf('_');
      int secondUnderscore = cmd.indexOf('_', firstUnderscore + 1);

      if (secondUnderscore > 0)
      {
        String numStr = cmd.substring(firstUnderscore + 1, secondUnderscore);
        String velStr = cmd.substring(secondUnderscore + 1);

        int noteNum = numStr.toInt();
        int noteVel = velStr.toInt();

        if (noteNum >= 0 && noteNum <= 127 && noteVel >= 0 && noteVel <= 127)
        {
          currentMidi.type = MSG_NOTE;
          currentMidi.inNumber = noteNum;
          currentMidi.inValue = noteVel;

          // Apply mapping
          MappedOutput outputs[10];
          int outputCount = 0;
          applyMapping(currentMidi, outputs, outputCount);

          // Display first output
          if (outputCount > 0)
          {
            currentMidi.type = outputs[0].type; // Update type in case it changed
            currentMidi.outNumber = outputs[0].number;
            currentMidi.outValue = outputs[0].value;
          }
          else
          {
            currentMidi.outNumber = noteNum;
            currentMidi.outValue = noteVel;
          }

          updateDisplay();
          Serial.printf("✓ Note %s Vel:%d -> ", NOTE_NAMES[noteNum], noteVel);

          // Print all outputs with their types
          for (int i = 0; i < outputCount; i++)
          {
            switch (outputs[i].type)
            {
            case MSG_CC:
              Serial.printf("CC%d:%d", outputs[i].number, outputs[i].value);
              break;
            case MSG_PC:
              Serial.printf("PC%d", outputs[i].number);
              break;
            case MSG_NOTE:
              Serial.printf("%s:%d", NOTE_NAMES[outputs[i].number], outputs[i].value);
              break;
            }
            if (i < outputCount - 1)
              Serial.print(", ");
          }
          Serial.println();
        }
        else
        {
          Serial.println("✗ Error: Note number and velocity must be 0-127");
        }
      }
      else
      {
        Serial.println("✗ Error: Format should be nn_<number>_<velocity>");
      }
    }
    else if (cmd == "help" || cmd == "?")
    {
      Serial.println("\n=== MIDI Mapper Commands ===");
      Serial.println("cc_<num>_<val>  - Control Change (e.g., cc_12_64)");
      Serial.println("pc_<num>        - Program Change (e.g., pc_5)");
      Serial.println("nn_<num>_<vel>  - Note (e.g., nn_60_100)");
      Serial.println("map             - Toggle mapping on/off");
      Serial.println("showmap         - Show current mappings");
      Serial.println("loadmap         - Load default mapping");
      Serial.println("demo            - Toggle demo mode");
      Serial.println("help or ?       - Show this help");
      Serial.println("===========================\n");
    }
    else if (cmd == "map")
    {
      mappingEnabled = !mappingEnabled;
      if (mappingEnabled)
      {
        Serial.println("✓ Mapping ENABLED - messages will be mapped");
      }
      else
      {
        Serial.println("✓ Mapping DISABLED - pass-through mode");
      }
    }
    else if (cmd == "showmap")
    {
      if (mapDoc.isNull() || mapDoc.size() == 0)
      {
        Serial.println("✗ No mapping loaded. Use 'loadmap' to load default mapping.");
      }
      else
      {
        Serial.println("\n=== Current Mapping JSON ===");
        serializeJsonPretty(mapDoc, Serial);
        Serial.println("\n===========================\n");
      }
    }
    else if (cmd == "loadmap")
    {
      loadMapping(defaultMapping);
    }
    else if (cmd == "demo")
    {
      demoEnabled = !demoEnabled;
      if (demoEnabled)
      {
        Serial.println("✓ Demo mode ENABLED - auto-cycling through MIDI messages");
      }
      else
      {
        Serial.println("✓ Demo mode DISABLED - use serial commands");
      }
    }
    else
    {
      Serial.println("✗ Unknown command. Type 'help' for command list");
    }
  }
}

void loop()
{
  // Check for serial commands
  parseSerialCommand();

  // Update LED state (turn off after duration)
  updateLED();

  // Demo: Cycle through different MIDI message types every 1 second (if enabled)
  static unsigned long lastUpdate = 0;
  static uint8_t demoMode = 0;

  if (demoEnabled && millis() - lastUpdate > 1000)
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

  // Parse any serial commands
  parseSerialCommand();
}