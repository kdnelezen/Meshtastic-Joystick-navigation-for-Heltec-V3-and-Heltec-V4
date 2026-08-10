# Meshtastic Joystick Navigation — Heltec V3 & V4

> **Add a 5-way joystick to your Heltec WiFi LoRa 32 (V3 or V4) and navigate
> the Meshtastic OLED UI without touching the device.**

[![PlatformIO](https://img.shields.io/badge/build-PlatformIO-orange)](https://platformio.org/)
[![Meshtastic](https://img.shields.io/badge/firmware-Meshtastic-blue)](https://meshtastic.org/)
[![Boards](https://img.shields.io/badge/boards-Heltec%20V3%20%7C%20V4-green)]()
[![License](https://img.shields.io/badge/license-MIT-lightgrey)](LICENSE)

---

## What this project is

This repository is a **Meshtastic firmware variant** for the Heltec WiFi LoRa 32
V3 and V4 boards.  It adds a `JoystickInput` driver to the official Meshtastic
firmware and wires it into the existing `InputBroker` event system, so the
normal Meshtastic OLED UI can be driven with a passive 5-way joystick.

It is no longer a standalone Arduino demo — it builds the real Meshtastic
firmware from the vendored `firmware/` directory.

---

## Hardware Requirements

### Boards

| Board | Chip | Flash / PSRAM | Display |
|-------|------|---------------|---------|
| Heltec WiFi LoRa 32 **V3** | ESP32-S3FN8 | 8 MB / 8 MB | 0.96″ SSD1306 OLED |
| Heltec WiFi LoRa 32 **V4** | ESP32-S3R2 | 8 MB / 2 MB | 0.96″ SSD1306 / SSD1315 OLED |

### Joystick

Any passive **5-way navigation joystick** switch.  The common pin goes to GND;
each direction pin goes to a free GPIO.  Internal pull-ups are enabled by the
firmware, so no external resistors are required.

| Joystick Pin | V3 GPIO | V4 GPIO |
|--------------|---------|---------|
| UP           | 1       | 15      |
| DOWN         | 2       | 16      |
| LEFT         | 4       | 19      |
| RIGHT        | 5       | 20      |
| CENTER       | 6       | 6       |
| GND (common) | GND     | GND     |

> The V4 defaults avoid LoRa radio pins (8–14), FEM control pins (2, 5, 7, 46),
> the OLED I2C bus (17–18) and GPS pins.  Always verify against your board
> silkscreen before wiring.

---

## Project Structure

```
.
├── firmware/                           # Vendored Meshtastic firmware tree
│   ├── src/
│   │   └── input/
│   │       ├── JoystickInput.h         # Joystick driver header
│   │       ├── JoystickInput.cpp       # Joystick driver implementation
│   │       └── InputBroker.cpp         # Hook that registers JoystickInput
│   └── variants/esp32s3/
│       ├── heltec_v3_joystick/
│       │   ├── variant.h               # V3 + joystick pin definitions
│       │   └── platformio.ini          # V3 joystick build environment
│       └── heltec_v4_joystick/
│           ├── variant.h               # V4 + joystick pin definitions
│           └── platformio.ini          # V4 joystick build environment
├── examples/
│   └── StandaloneJoystick/
│       └── StandaloneJoystick.ino      # Minimal wiring test (no mesh)
├── docs/
│   └── wiring.md                       # Detailed wiring diagrams
├── flash_tool/
│   └── index.html                      # Web flasher (firmware binaries must
│                                       # be built first and copied here)
├── scripts/
│   └── pre_build.py                    # Legacy helper
├── LICENSE
└── README.md                           # This file
```

---

## Build & Flash

### Prerequisites

- Python 3.8+
- PlatformIO Core (`pip install platformio`)
- Git

### Build

```bash
cd firmware

# Heltec V3 + joystick
pio run -e heltec-v3-joystick

# Heltec V4 + joystick
pio run -e heltec-v4-joystick
```

### Flash

Connect the board via USB, then:

```bash
# Heltec V3
pio run -e heltec-v3-joystick --target upload

# Heltec V4
pio run -e heltec-v4-joystick --target upload
```

For a first-time install it is recommended to erase the flash first:

```bash
pio run -e heltec-v3-joystick --target erase
pio run -e heltec-v3-joystick --target upload
```

### Serial monitor

```bash
pio device monitor --baud 115200
```

---

## Joystick Controls

| Joystick Action | Meshtastic UI Action |
|-----------------|----------------------|
| **UP**          | Scroll up / move selection up |
| **DOWN**        | Scroll down / move selection down |
| **LEFT**        | Previous screen / back |
| **RIGHT**       | Next screen / forward |
| **CENTER tap**  | Confirm / Enter / Select |
| **CENTER hold** | Back / Cancel |

The driver uses the standard `InputBroker` event types, so every existing
Meshtastic screen that works with a rotary encoder or trackball also works
with the joystick.

---

## Changing the GPIO pins

Edit the joystick pins in the relevant variant header:

- `firmware/variants/esp32s3/heltec_v3_joystick/variant.h`
- `firmware/variants/esp32s3/heltec_v4_joystick/variant.h`

```cpp
#define JOYSTICK_PIN_UP     1
#define JOYSTICK_PIN_DOWN   2
#define JOYSTICK_PIN_LEFT   4
#define JOYSTICK_PIN_RIGHT  5
#define JOYSTICK_PIN_CENTER 6
```

You can also adjust debounce and long-press timing:

```cpp
#define JOYSTICK_DEBOUNCE_MS   50
#define JOYSTICK_LONG_PRESS_MS 800
```

---

## Standalone wiring test

If you want to verify the joystick wiring before building the full firmware,
flash the minimal example in `examples/StandaloneJoystick/`.  It prints each
joystick event to the serial console and does not require the Meshtastic
firmware tree.

---

## Updating the vendored firmware

The `firmware/` directory contains a snapshot of the official Meshtastic
firmware.  To update it to a newer release:

```bash
cd firmware
git fetch origin
git checkout <release-tag>
git submodule update --init --recursive
cd ..
git add firmware
```

---

## Troubleshooting

### Build fails downloading packages

Make sure PlatformIO can reach the package registry and GitHub.  On some
networks you may need to configure a proxy or retry after network issues.

### Board not detected

- Use a USB cable with data lines (not charge-only).
- Install the SiLabs CP2102 VCP driver on Windows/macOS.
- On Linux, add your user to the `dialout` group.

### Joystick direction not responding

- Check wiring: each direction pin → GPIO, common → GND.
- Verify the GPIO numbers match the variant header.
- Open the serial monitor and push the joystick — you should see
  `[JOY] event=...` logged.

### Display blank / scrambled

- Ensure `VEXT_ENABLE` is driven LOW before I2C init (the variant/firmware
  handles this).
- Confirm `RESET_OLED`, `I2C_SDA` and `I2C_SCL` match your board.

---

## Contributing

Pull requests are welcome.  Please keep joystick-specific changes isolated to
`JoystickInput.*` and the `heltec_v*_joystick` variant directories so the
vendored firmware remains easy to update.

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Acknowledgements

- [Meshtastic Project](https://meshtastic.org) — open-source mesh radio firmware
- [Heltec Automation](https://heltec.org) — ESP32+LoRa hardware
- [Espressif Systems](https://espressif.com) — ESP32-S3 platform
- [PlatformIO](https://platformio.org) — embedded build system
