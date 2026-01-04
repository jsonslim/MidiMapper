# Cross-Type MIDI Mapping - Advanced Feature Guide

## 🔀 Overview

The MIDI Mapper now supports **cross-type mappings**, allowing you to map between different MIDI message types:

- **CC → Note:** Control Change to Note On
- **Note → CC:** Note to Control Change
- **PC → CC/Note:** Program Change to CC or Note
- **Any → Any:** Full flexibility!

## 🎯 Why Cross-Type Mapping?

### Use Case Examples:

1. **Drum Pad to CC:** Map drum pad hits (notes) to control synth parameters (CC)
2. **Fader to Notes:** Use a fader (CC) to trigger melodic notes
3. **Program Change to CC:** Switching presets also changes parameters
4. **CC to Multiple Types:** One knob controls CC filter + triggers note

## 📝 Syntax Options

### Option 1: String Format

Use `"type:number"` syntax in mappings.

**Format:**

```json
"input_number": "output_type:output_number"
```

**Supported Types:**

- `"cc:N"` - Control Change
- `"pc:N"` - Program Change
- `"note:N"` or `"nn:N"` - Note

**Examples:**

```json
{
  "cc_map": {
    "23": "note:45" // CC23 → Note 45 (A2)
  },
  "note_map": {
    "60": "cc:74" // Note C4 → CC74
  },
  "pc_map": {
    "5": "note:72" // PC5 → Note C5
  }
}
```

### Option 2: Object Format

Use `"type"` field in object mapping.

**Format:**

```json
"input_number": {
  "type": "cc|pc|note",
  "num": number,
  "scale": optional_scale
}
```

**Examples:**

```json
{
  "cc_map": {
    "1": {
      "type": "note",
      "num": 64,
      "scale": 1.0
    }
  },
  "note_map": {
    "72": {
      "type": "cc",
      "num": 76,
      "velocity": 1.2
    }
  }
}
```

### Option 3: Mixed Arrays

Combine same-type and cross-type in arrays.

**Examples:**

```json
{
  "cc_map": {
    "74": [71, 72, "note:60"] // CC74 → CC71, CC72, Note C4
  },
  "pc_map": {
    "5": ["cc:74", "note:60"] // PC5 → CC74, Note C4
  },
  "note_map": {
    "62": ["cc:74", "note:67"] // Note D4 → CC74, Note G4
  }
}
```

## 🎼 Complete Examples

### Example 1: CC23 → Note A2 (45)

**JSON:**

```json
{
  "cc_map": {
    "23": "note:45"
  }
}
```

**Test:**

```
> cc_23_112
✓ CC23 Value:112 -> Note A2:112
```

**Result:**

- Input: CC23 with value 112
- Output: Note A2 (45) with velocity 112
- Display shows: "Note A2" with value 112

### Example 2: Drum Pad (Note) to Filter (CC)

**JSON:**

```json
{
  "note_map": {
    "36": "cc:74", // Kick drum → Filter cutoff
    "38": "cc:71", // Snare → Filter resonance
    "42": "cc:72" // Hi-hat → LFO rate
  }
}
```

**Test:**

```
> nn_36_100
✓ Note C1 Vel:100 -> CC74:100

> nn_38_80
✓ Note D1 Vel:80 -> CC71:80
```

### Example 3: Multi-Type Output

**JSON:**

```json
{
  "cc_map": {
    "1": [11, "note:60", "pc:10"]
  }
}
```

**Test:**

```
> cc_1_64
✓ CC1 Value:64 -> CC11:64, Note C4:64, PC10
```

**Result:** One CC triggers three different message types!

### Example 4: Program Change with Actions

**JSON:**

```json
{
  "pc_map": {
    "0": ["cc:7", "note:60"], // PC0 → Set volume + play C4
    "1": ["cc:10", "note:64"] // PC1 → Set pan + play E4
  }
}
```

**Test:**

```
> pc_0
✓ PC0 -> CC7:0, Note C4:0
```

### Example 5: CC to Note with Scaling

**JSON:**

```json
{
  "cc_map": {
    "1": {
      "type": "note",
      "num": 60,
      "scale": 0.8
    }
  }
}
```

**Test:**

```
> cc_1_127
✓ CC1 Value:127 -> Note C4:101
```

_(127 × 0.8 = 101.6 → 101)_

## 📊 Mapping Behavior

### Value Handling

- **CC → Note:** CC value becomes note velocity
- **Note → CC:** Note velocity becomes CC value
- **PC → CC/Note:** PC has no value, outputs get value 0 or inherit from context
- **Any → PC:** PC ignores values (Program Changes don't have values)

### Display Behavior

When output type changes:

```
Input Type:  CC
Output Type: Note
Display:     Shows "Note A2" instead of "CC45"
```

The display automatically adapts to show the correct format for the output type!

## 🎯 Real-World Scenarios

### Scenario 1: DJ Controller Mapping

**Problem:** DJ controller sends notes, but DAW needs CCs for filters.

**Solution:**

```json
{
  "note_map": {
    "48": "cc:74", // Pad 1 → Filter cutoff
    "49": "cc:71", // Pad 2 → Resonance
    "50": "cc:91", // Pad 3 → Reverb
    "51": "cc:93" // Pad 4 → Chorus
  }
}
```

### Scenario 2: Step Sequencer to Synth

**Problem:** Step sequencer uses CCs, synth needs notes.

**Solution:**

```json
{
  "cc_map": {
    "20": "note:60", // Step 1 → C4
    "21": "note:62", // Step 2 → D4
    "22": "note:64", // Step 3 → E4
    "23": "note:65" // Step 4 → F4
  }
}
```

### Scenario 3: Preset with Parameter Changes

**Problem:** Need to change multiple parameters when switching presets.

**Solution:**

```json
{
  "pc_map": {
    "0": ["cc:7", "cc:74", "cc:71"], // Bright preset
    "1": ["cc:7", "cc:74", "cc:71"] // Dark preset
  }
}
```

### Scenario 4: Velocity-Sensitive Filter

**Problem:** Want note velocity to control filter cutoff.

**Solution:**

```json
{
  "note_map": {
    "60": [60, "cc:74"] // Play note + control filter
  }
}
```

## 🔍 Technical Details

### Type Resolution

```cpp
// Input type stored in midi.type
// Output type determined by mapping:
outputs[i].type = MSG_CC    // or MSG_PC, MSG_NOTE

// Display updates based on output type:
if (outputs[0].type == MSG_NOTE) {
  show_note_name(outputs[0].number);
}
```

### Value Preservation

Values are preserved across type conversions:

- CC value 100 → Note velocity 100
- Note velocity 80 → CC value 80
- PC (no value) → outputs get value 0

### Scaling with Type Conversion

Scaling works across types:

```json
{
  "cc_map": {
    "1": {
      "type": "note",
      "num": 60,
      "scale": 0.5
    }
  }
}
```

CC1 value 127 → Note C4 velocity 63 (127 × 0.5 = 63.5)

## 📺 Serial Output Format

### Cross-Type Output Examples:

```
> cc_23_112
✓ CC23 Value:112 -> Note A2:112

> nn_60_100
✓ Note C4 Vel:100 -> CC74:100

> pc_5
✓ PC5 -> CC74:0, Note C4:0

> cc_74_80
✓ CC74 Value:80 -> CC71:80, CC72:80, Note C4:80
```

## 🎨 Display Examples

### Before (Same Type):

```
IN: CC12    OUT: CC16
    64          64
```

### After (Cross Type):

```
IN: CC23    OUT: A2
    112         112
```

```
IN: C4      OUT: CC74
    100         100
```

## 💡 Tips & Best Practices

1. **Value Ranges:** Remember MIDI values are 0-127 for all types
2. **PC Values:** Program Changes don't use values, they'll output 0
3. **Mixed Arrays:** Great for creating "macro" controls
4. **Scaling:** Use when converting between different parameter ranges
5. **Testing:** Use serial commands to test mappings before deploying

## ⚠️ Limitations

- PC messages don't carry values (outputs will have value 0)
- Max 10 outputs per input
- Type string format is case-insensitive but must be exact: "cc", "pc", "note", or "nn"
- Display shows only first output (serial shows all)

## 🚀 Advanced Patterns

### Pattern 1: Macro Control

One CC controls filter, resonance, and plays note:

```json
{ "74": ["cc:71", "cc:72", "note:60"] }
```

### Pattern 2: Velocity Doubling

Note triggers itself AND doubles velocity to CC:

```json
{ "60": [60, { "type": "cc", "num": 74, "velocity": 2.0 }] }
```

### Pattern 3: Preset Chain

PC triggers multiple PCs:

```json
{ "0": ["pc:10", "pc:20", "pc:30"] }
```

---

**Status:** ✅ Fully implemented with cross-type support  
**Compatibility:** All MIDI message types (CC, PC, Note)  
**Performance:** Real-time with no noticeable latency
