# ESP32-C3 MIDI Mapper - Current Project State

_Last Updated: January 4, 2026_

## 📋 Project Overview

This is an ESP32-C3 based MIDI device with an ST7789 display (172x320px with rounded corners) using the LovyanGFX library. The current implementation displays a MIDI mapping interface showing INPUT and OUTPUT MIDI CC (Control Change) messages.

---

## 🔧 Hardware Configuration

### ESP32-C3 Board

- **Board:** DFRobot Beetle ESP32-C3
- **Framework:** Arduino
- **Platform:** Espressif32

### ST7789 Display

- **Resolution:** 172x320 pixels (visible area)
- **Physical Dimensions:** Rounded corners (23px radius)
- **Offset:** X=34, Y=0
- **Rotation:** offset_rotation = 2, display rotation = 1 (landscape)
- **Current Orientation:** Landscape mode (320x172 effective)

#### Display Pin Connections

| Function | GPIO | Description          |
| -------- | ---- | -------------------- |
| CS       | 5    | Chip Select          |
| DC       | 2    | Data/Command         |
| RES      | 4    | Reset                |
| SDA      | 10   | MOSI (SPI Data)      |
| SCL      | 8    | SCLK (SPI Clock)     |
| BL       | -1   | Backlight (not used) |

### MIDI Interface (Serial1)

| Function | GPIO | Baud Rate |
| -------- | ---- | --------- |
| TX       | 6    | 31250     |
| RX       | 7    | 31250     |

---

## 📁 Project Structure

```
MIDI_MAPPER/
├── platformio.ini              # PlatformIO configuration
├── src/
│   ├── main.cpp               # Main application (MIDI mapper UI)
│   ├── DisplayDrv_st7789.h    # ST7789 display driver
│   └── globals.h              # Pin definitions
├── data/
│   └── midiMap.json           # MIDI mapping configuration
├── include/
├── lib/
├── test/
├── EEZ_Project/               # EEZ Studio UI project
└── ReactUI/                   # React-based web UI
```

---

## 🎨 Current Display Interface

### Layout Description

The display shows a MIDI CC mapping interface in landscape mode:

```
┌─────────────────────────────────────┐
│ IN                          OUT     │  ← Headers (Green)
├─────────────────────────────────────┤
│                                     │
│  CC12                      CC16     │  ← CC Numbers (Cyan, Size 4)
│                                     │
│  123                       40       │  ← Values (Yellow, Size 4)
│                                     │
└─────────────────────────────────────┘
   Magenta rounded border (23px radius)
```

### UI Elements

#### Text Display

| Element   | Position   | Size | Color      | Content                |
| --------- | ---------- | ---- | ---------- | ---------------------- |
| IN Label  | (20, 10)   | 2    | Dark Green | "IN"                   |
| OUT Label | (190, 10)  | 2    | Dark Green | "OUT"                  |
| IN CC     | (20, 56)   | 4    | Cyan       | "CC12"                 |
| OUT CC    | (190, 56)  | 4    | Cyan       | "CC16"                 |
| IN Value  | (20, 105)  | 4    | Yellow     | "123"                  |
| OUT Value | (190, 105) | 4    | Yellow     | "40"                   |
| Status    | (120, 300) | 1    | White      | "_ READY _" (blinking) |

#### Graphics

- **Border:** Rounded rectangle (0, 0, 320, 172) with 23px corner radius in Magenta
- **Status Indicator:** Blinking "_ READY _" text (1-second interval)

### Commented Out Elements

Several visual elements are commented out and available for future use:

- Triangle indicators (for mapping direction)
- Horizontal line separator
- Vertical divider line
- Additional triangles and connection lines

---

## 📦 Dependencies

### Active Libraries

| Library      | Version | Purpose                       |
| ------------ | ------- | ----------------------------- |
| LovyanGFX    | ^1.2.0  | Display graphics library      |
| MIDI Library | ^5.0.2  | MIDI protocol handling        |
| ArduinoJson  | ^7.4.1  | JSON parsing for MIDI mapping |

### Removed Libraries

- `ESPAsyncWebServer` - Removed (caused WiFiServer.h error)
- `AsyncTCP` - Removed (dependency of web server)
- `lvgl` - Removed (not needed for current implementation)

---

## 💻 Code Analysis

### main.cpp Structure

#### `setup()` Function

1. **Serial Initialization:** 115200 baud for debugging
2. **MIDI Serial Initialization:** Serial1 at 31250 baud on GPIO6/7
3. **Display Initialization:**
   - Initialize display
   - Set rotation to 1 (landscape)
   - Clear screen (black background)
4. **Draw UI:**
   - IN/OUT labels
   - CC numbers (CC12 → CC16)
   - Current values (123 → 40)
   - Rounded border frame

#### `loop()` Function

- **Blinking Status Indicator:**
  - Toggles "_ READY _" text visibility every 1000ms
  - Uses non-blocking timing with `millis()`

### DisplayDrv_st7789.h

- Custom LovyanGFX device class
- Configured for ESP32-C3 SPI2_HOST
- Panel parameters optimized for 172x320 display with offsets
- 40MHz write speed, 16MHz read speed
- Invert enabled for proper color display

### globals.h

- Centralized pin definitions
- Easy to modify hardware configuration
- Separates hardware config from logic

---

## 🔍 Display Characteristics

### Visible Area

- **Panel Width:** 172 pixels (physical visible width)
- **Panel Height:** 320 pixels (physical visible height)
- **X Offset:** 34 pixels (accounts for left/right rounded corners)
- **Y Offset:** 0 pixels

### Rotation Settings

- **offset_rotation:** 2 (in driver configuration)
- **setRotation:** 1 (in main.cpp)
- **Result:** 320x172 landscape display (width x height)

### Rounded Corners

- **Physical corner radius:** ~23 pixels
- **Border drawn at:** (0, 0, 320, 172) with 23px radius
- **Note:** Border follows physical display shape

---

## 🚀 Current Functionality

### Implemented

✅ Display initialization and configuration  
✅ MIDI Serial1 hardware setup (ready for MIDI I/O)  
✅ MIDI CC mapping UI display  
✅ Real-time status indicator (blinking READY)  
✅ Proper handling of rounded display corners  
✅ Color-coded interface (Green/Cyan/Yellow/Magenta)

### Not Yet Implemented

⏳ Actual MIDI message reception and transmission  
⏳ Dynamic updating of CC numbers and values  
⏳ MIDI mapping table loading from JSON  
⏳ User interaction (buttons/touch)  
⏳ Multiple mapping pages/presets

---

## 🛠️ Build & Upload

### Commands

```bash
# Build the project
pio run

# Upload to ESP32-C3
pio run --target upload

# Monitor serial output
pio device monitor

# Build, upload and monitor
pio run -t upload && pio device monitor
```

### Expected Serial Output

```
ESP32-C3 ST7789 Display Test
MIDI Serial1 initialized on TX:GPIO6, RX:GPIO7
Display initialized
Hello World displayed!
```

---

## 🎯 Next Steps / Roadmap

### Phase 1: MIDI Functionality

1. Implement MIDI message reception on Serial1
2. Parse incoming MIDI CC messages
3. Load MIDI mapping table from `data/midiMap.json`
4. Apply CC number transformations
5. Transmit mapped MIDI messages on Serial1

### Phase 2: Dynamic Display Updates

1. Update IN CC number and value when MIDI received
2. Update OUT CC number and value after mapping
3. Add visual feedback for MIDI activity (LED/indicator)
4. Implement scrolling or paging for multiple mappings

### Phase 3: User Interface Enhancement

1. Add buttons for preset selection
2. Implement visual mapping indicators (arrows/lines)
3. Add configuration mode
4. Display MIDI channel information

### Phase 4: Web Interface Integration

1. Re-enable WiFi/WebServer (if needed)
2. Web-based configuration interface
3. Upload/download mapping presets
4. Real-time monitoring via web browser

---

## 📝 Notes

- The display's rounded corners require careful positioning of UI elements
- Rounded rectangle border at 23px radius matches physical display shape
- Current UI is static; next step is to make it dynamic based on MIDI I/O
- Serial1 is configured and ready for MIDI communication
- The MIDI Library and ArduinoJson are ready but not yet utilized in code

---

## 🐛 Known Issues / Considerations

1. **Backlight:** Currently set to -1 (not controlled). If display is dim, connect backlight pin and update `TFT_BL_PIN` in `globals.h`

2. **Display Offsets:** The offset_x=34 compensates for rounded corners. If display appears shifted, adjust this value in `DisplayDrv_st7789.h`

3. **Color Inversion:** `cfg.invert = true` is set. If colors look wrong, toggle this to `false`

4. **Blinking Text Position:** Currently at (120, 300) which may be off-screen. Adjust if needed based on actual rotation

---

## 📄 License

This project uses open-source libraries. Check individual library licenses for details.
