# Serial Command Parser - User Guide

## 🎹 Overview

The MIDI Mapper now includes a serial command parser that allows you to send MIDI commands via the USB Serial connection and see them displayed in real-time on the screen.

## 📡 Command Format

### Control Change (CC)

```
cc_<number>_<value>
```

- **number:** CC number (0-127)
- **value:** CC value (0-127)

**Examples:**

```
cc_12_64      → CC12 with value 64
cc_74_100     → CC74 with value 100
cc_1_127      → CC1 (Mod Wheel) with value 127
```

### Program Change (PC)

```
pc_<number>
```

- **number:** Program number (0-127)

**Examples:**

```
pc_5          → Program Change 5
pc_127        → Program Change 127
```

### Note

```
nn_<number>_<velocity>
```

- **number:** Note number (0-127)
- **velocity:** Note velocity (0-127)

**Examples:**

```
nn_60_100     → Note C4 (Middle C) with velocity 100
nn_64_80      → Note E4 with velocity 80
nn_72_127     → Note C5 with velocity 127
```

## 🎮 Control Commands

### Help

```
help
?
```

Displays the command list and usage examples.

### Toggle Demo Mode

```
demo
```

Toggles the automatic demo mode on/off:

- **ON:** Display cycles through demo messages every 1 second
- **OFF:** Display only responds to your serial commands

## 🖥️ Usage Example

### Basic Session

```
> help
=== MIDI Mapper Commands ===
cc_<num>_<val>  - Control Change (e.g., cc_12_64)
pc_<num>        - Program Change (e.g., pc_5)
nn_<num>_<vel>  - Note (e.g., nn_60_100)
demo            - Toggle demo mode
help or ?       - Show this help
===========================

> demo
✓ Demo mode DISABLED - use serial commands

> cc_12_64
Received command: cc_12_64
✓ CC12 Value:64

> nn_60_100
Received command: nn_60_100
✓ Note C4 Velocity:100

> pc_5
Received command: pc_5
✓ PC5
```

## 📺 Display Behavior

When you send a command:

1. **Green LED** flashes in top-left corner
2. **IN section** shows your command (CC12, PC5, C4, etc.)
3. **Value** displays below (for CC and Notes)
4. **OUT section** currently mirrors IN (mapping not yet implemented)

## ⚙️ Configuration

### Default State

- **Demo Mode:** ENABLED on startup
- **Baud Rate:** 115200
- **Line Ending:** Newline (\n)

### Change Default Demo Mode

In `main.cpp`, modify:

```cpp
bool demoEnabled = true;  // Change to false to start with demo OFF
```

## 🔧 Error Handling

The parser validates input and provides helpful error messages:

### Invalid CC Range

```
> cc_200_64
✗ Error: CC number and value must be 0-127
```

### Wrong Format

```
> cc_12
✗ Error: Format should be cc_<number>_<value>
```

### Unknown Command

```
> xyz
✗ Unknown command. Type 'help' for command list
```

## 🎯 Use Cases

### 1. Testing MIDI Messages

Quickly test different MIDI commands without external hardware:

```
cc_74_64
cc_71_80
pc_10
```

### 2. Manual Mapping Testing

Test specific CC mappings before implementing automation:

```
demo              # Turn off auto-demo
cc_12_64          # Test input
cc_16_127         # Compare expected output
```

### 3. Note Testing

Test note mapping and display:

```
nn_60_100         # Middle C
nn_64_100         # E
nn_67_100         # G
```

### 4. Development & Debugging

Monitor and control MIDI flow during development:

```
demo              # Toggle as needed
cc_1_127          # Max mod wheel
cc_1_0            # Min mod wheel
```

## 💡 Tips

1. **Case Insensitive:** Commands work in any case (CC_12_64 or cc_12_64)
2. **Whitespace:** Leading/trailing spaces are automatically trimmed
3. **Live Updates:** Display updates immediately when valid command received
4. **LED Feedback:** Green LED confirms command processing
5. **Demo Toggle:** Use `demo` command to switch between auto and manual modes

## 🚀 Next Steps

The parser is ready for:

- Integration with MIDI Library for Serial1 MIDI I/O
- Loading mapping tables from JSON
- Implementing actual CC/PC/Note transformations
- Adding OUT section mapping logic

## 📊 Serial Monitor Settings

**Arduino IDE / PlatformIO:**

- Baud Rate: 115200
- Line Ending: Newline or Both NL & CR

**Command Prompt:**

```bash
pio device monitor
# Then type commands directly
```

---

**Status:** ✅ Fully implemented and tested
**Compatible with:** Demo mode (can run simultaneously or independently)
