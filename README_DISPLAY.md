# ESP32-C3 MIDI Mapper with ST7789 Display

## Project Overview

This is an ESP32-C3 based MIDI device with an ST7789 display using the LovyanGFX library.

## Hardware Configuration

### ESP32-C3 Board

- Board: DFRobot Beetle ESP32-C3
- Framework: Arduino

### ST7789 Display Connections

- **CS (Chip Select):** GPIO5
- **DC (Data/Command):** GPIO2
- **RES (Reset):** GPIO4
- **SDA (MOSI):** GPIO10
- **SCL (SCLK):** GPIO8
- **Display Size:** 240x320 pixels

### MIDI Serial Port (Serial1)

- **TX:** GPIO6
- **RX:** GPIO7
- **Baud Rate:** 31250 (MIDI standard)

## Project Structure

```
src/
├── main.cpp              # Main sketch with Hello World display demo
├── DisplayDrv_st7789.h   # ST7789 display driver using LovyanGFX
└── globals.h             # Pin definitions and global configuration
```

## Key Features of Current Sketch

### Setup Function

1. Initializes Serial debugging at 115200 baud
2. Initializes Serial1 for MIDI communication at 31250 baud
3. Initializes ST7789 display
4. Displays "Hello World" with colorful text
5. Shows ESP32-C3, ST7789 Display, and LovyanGFX labels
6. Draws decorative shapes (rectangle, circles)

### Loop Function

- Displays a blinking "_ READY _" message at the bottom
- Blinks every 1 second

## Display Features Demonstrated

1. **Text Display:** Multiple text sizes and colors
2. **Color Support:** White, Green, Cyan, Yellow, Magenta, Red, Blue
3. **Graphics:** Rectangles and circles
4. **Screen Rotation:** Set to landscape mode (rotation 1)
5. **Dynamic Updates:** Blinking text in loop

## Building and Uploading

### Using PlatformIO CLI:

```bash
# Build the project
pio run

# Upload to ESP32-C3
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using PlatformIO IDE:

1. Click the "Build" button (checkmark icon)
2. Click the "Upload" button (arrow icon)
3. Click the "Serial Monitor" button to see debug output

## Expected Output

When the sketch runs, you should see:

- **On Serial Monitor:**

  ```
  ESP32-C3 ST7789 Display Test
  MIDI Serial1 initialized on TX:GPIO6, RX:GPIO7
  Display initialized
  Hello World displayed!
  ```

- **On Display:**
  - "Hello World!" in large white text
  - "ESP32-C3" in green
  - "ST7789 Display" in cyan
  - "LovyanGFX" in yellow
  - Magenta border rectangle
  - Blue filled circle at bottom
  - Blinking "_ READY _" text

## Troubleshooting

### Display Not Working

1. **Check Connections:** Verify all GPIO pins are correctly connected
2. **Check Power:** Ensure display has adequate power supply
3. **Adjust Display Settings:** In `DisplayDrv_st7789.h`, you may need to adjust:
   - `cfg.invert` (try false instead of true)
   - `cfg.rgb_order` (try true instead of false)
   - `cfg.offset_rotation` (try values 0-3)
   - SPI frequency (try 20MHz: `cfg.freq_write = 20000000`)

### Display Colors Wrong

- Toggle `cfg.rgb_order` between true and false
- Toggle `cfg.invert` between true and false

### Display Upside Down or Rotated

- Change `tft.setRotation(1)` in main.cpp to 0, 2, or 3

### MIDI Not Working

- Verify Serial1 connections (TX=GPIO6, RX=GPIO7)
- Check MIDI circuit has proper optocoupler isolation
- Verify 31250 baud rate is correct

## Next Steps

To extend this project, you can:

1. Add MIDI message handling in the loop
2. Display MIDI data on screen
3. Create a UI for MIDI mapping configuration
4. Add touch screen support (if available)
5. Implement menu system for device settings

## Dependencies

- **LovyanGFX:** ^1.2.0 - Display graphics library
- **ArduinoJson:** ^7.4.1 - JSON parsing (for MIDI mapping)
- **MIDI Library:** ^5.0.2 - MIDI protocol handling
- **ESPAsyncWebServer:** ^3.7.6 (optional, for future web config)
- **LVGL:** ^8.1.0 (optional, advanced UI)

## License

This project uses open-source libraries. Check individual library licenses for details.
