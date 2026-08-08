# Meshtastic Joystick Navigation — Heltec V3 & V4

> **Add a 5-way joystick to your Heltec WiFi LoRa 32 (V3 or V4) for full
> hands-free navigation of the Meshtastic mesh-radio interface.**

[![PlatformIO](https://img.shields.io/badge/build-PlatformIO-orange)](https://platformio.org/)
[![Meshtastic](https://img.shields.io/badge/firmware-Meshtastic%202.7.x-blue)](https://meshtastic.org/)
[![Boards](https://img.shields.io/badge/boards-Heltec%20V3%20%7C%20V4-green)]()
[![License](https://img.shields.io/badge/license-MIT-lightgrey)](LICENSE)

---

## Table of Contents

1. [Features](#features)
2. [Hardware Requirements](#hardware-requirements)
3. [Joystick Wiring](#joystick-wiring)
4. [Quick-Start: Web Flash Tool](#quick-start-web-flash-tool)
5. [Build & Flash with PlatformIO](#build--flash-with-platformio)
6. [Meshtastic Feature Overview](#meshtastic-feature-overview)
7. [Joystick Navigation Controls](#joystick-navigation-controls)
8. [Project Structure](#project-structure)
9. [Integration with Full Meshtastic Firmware](#integration-with-full-meshtastic-firmware)
10. [Troubleshooting](#troubleshooting)
11. [FAQ](#faq)
12. [Contributing](#contributing)
13. [License](#license)

---

## Features

| Feature | Details |
|---------|---------|
| **Board support** | Heltec WiFi LoRa 32 V3 (ESP32-S3FN8) and V4 (ESP32-S3R2) |
| **Radio** | Semtech SX1262 LoRa — all ISM bands (433 / 868 / 915 MHz) |
| **Display** | 0.96″ SSD1306 OLED (128 × 64) with joystick-driven menus |
| **Firmware base** | Meshtastic stable v2.7.x — full feature set |
| **Input** | Passive 5-way joystick (UP / DOWN / LEFT / RIGHT / CENTER) |
| **Long-press** | CENTER held ≥ 800 ms → BACK / cancel |
| **Debounce** | Configurable (default 50 ms) |
| **Build system** | PlatformIO (VS Code or CLI) |
| **Web flasher** | Browser-based flash tool (Chrome/Edge 89+, no drivers needed) |
| **Standalone demo** | Runs without the full Meshtastic tree for rapid testing |

---

## Hardware Requirements

### Boards

| Board | Chip | Flash/PSRAM | Sleep current |
|-------|------|-------------|---------------|
| Heltec WiFi LoRa 32 **V3** | ESP32-S3FN8 | 8 MB / 8 MB | ~180 µA |
| Heltec WiFi LoRa 32 **V4** | ESP32-S3R2 | 8 MB / 2 MB | ~140 µA |

Both boards include:
- Semtech SX1262 LoRa radio
- 0.96″ SSD1306 OLED display
- USB-C port (CP2102 USB-UART bridge)
- JST-PH battery connector
- IPEX antenna connector

### Joystick

Any passive **5-way navigation joystick** switch:

| Recommended part | Type |
|------------------|------|
| RKJXK series (Alps) | Through-hole / SMD |
| SKRHAAE series (Alps) | SMD, very low profile |
| PSP-style joystick breakout | Breakout board, breadboard-friendly |
| Grove 5-way Switch | Grove connector |

No resistors required — the firmware enables **internal pull-ups**.

### Other

- USB-A to USB-C cable (USB-C to USB-C may cause charging issues)
- Breadboard or custom PCB for joystick mounting
- 5 × short wires / jumper cables

---

## Joystick Wiring

Connect the joystick common/GND pin to **GND** and each direction pin to the
corresponding GPIO on the Heltec header:

| Joystick Pin | GPIO | Heltec Header Label |
|--------------|------|---------------------|
| UP           | 1    | IO1                 |
| DOWN         | 2    | IO2                 |
| LEFT         | 4    | IO4                 |
| RIGHT        | 5    | IO5                 |
| CENTER/CLICK | 6    | IO6                 |
| GND (common) | GND  | GND                 |
| VCC (if needed) | 3V3 | 3V3              |

> **Active-LOW logic.** Pressing a direction pulls the GPIO LOW.
> Internal pull-ups are enabled automatically by the firmware.

Full wiring diagram with ASCII art and troubleshooting: **[docs/wiring.md](docs/wiring.md)**

### Quick wiring diagram

```
Heltec V3 / V4        5-way Joystick
──────────────        ──────────────
  3V3 ──────────────── VCC  (only if needed)
  GND ──────────────── COM / GND
  IO1 ──────────────── UP
  IO2 ──────────────── DOWN
  IO4 ──────────────── LEFT
  IO5 ──────────────── RIGHT
  IO6 ──────────────── CENTER
```

---

## Quick-Start: Web Flash Tool

The easiest way to flash your board — no software installation needed.

### Requirements

- Google Chrome 89+ or Microsoft Edge 89+ (desktop)
- CP2102 USB driver (see below if port not detected)

### Steps

1. **Open the flash tool:**
   Serve the repository locally, for example with `python3 -m http.server 8080`,
   then open `http://localhost:8080/flash_tool/` in Chrome/Edge.

2. **Select your board:** Click **Heltec V3** or **Heltec V4**.

3. **Connect the board:** Click **Connect via USB**, select the COM/ttyUSB port.

4. **Flash:** Click **Flash Firmware**.
   Check *Full erase before flash* for first-time installs (recommended).

5. **Done:** The board resets automatically and Meshtastic starts.

### Install CP2102 driver (if port not detected)

- **Windows / macOS:** [SiLabs CP2102 VCP drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- **Linux:** Driver is included in the kernel; add yourself to the `dialout` group:
  `sudo usermod -a -G dialout $USER` then log out/in.

> **Note:** Web Serial requires a secure context, so opening `flash_tool/index.html`
> directly via `file://` will not work.
>
> The web flash tool uses the pre-built firmware binaries in
> `flash_tool/firmware/`. Build them with PlatformIO first (see next section)
> or download release binaries from the [Releases page](../../releases).

---

## Build & Flash with PlatformIO

### Prerequisites

| Tool | Install |
|------|---------|
| Python 3.8+ | https://python.org |
| PlatformIO Core | `pip install platformio` |
| Git | https://git-scm.com |

VS Code users: install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode).

### Clone the repository

```bash
git clone https://github.com/kdnelezen/Meshtastic-Joystick-navigation-for-Heltec-V3-and-Heltec-V4.git
cd Meshtastic-Joystick-navigation-for-Heltec-V3-and-Heltec-V4
```

### Build

```bash
# Heltec V3
pio run -e heltec_v3_joystick

# Heltec V4
pio run -e heltec_v4_joystick
```

### Upload (flash)

Connect the board via USB, then:

```bash
# Heltec V3
pio run -e heltec_v3_joystick --target upload

# Heltec V4
pio run -e heltec_v4_joystick --target upload
```

PlatformIO auto-detects the serial port. To specify manually:

```bash
pio run -e heltec_v3_joystick --target upload --upload-port /dev/ttyUSB0
```

### Full erase before flashing (recommended for first install)

```bash
pio run -e heltec_v3_joystick --target erase
pio run -e heltec_v3_joystick --target upload
```

### Monitor serial output

```bash
pio device monitor --baud 115200
```

### VS Code (PlatformIO IDE)

1. Open the cloned folder in VS Code.
2. PlatformIO auto-detects `platformio.ini`.
3. Use the **PlatformIO sidebar** → select environment → click **Build** or **Upload**.

---

## Meshtastic Feature Overview

This firmware enables the full Meshtastic v2.7.x feature set:

| Category | Features |
|----------|----------|
| **Mesh networking** | Multi-hop LoRa mesh, automatic routing, store-and-forward |
| **Messaging** | Send/receive text messages to individuals or the whole mesh |
| **Node tracking** | Node list with GPS coordinates, distance, last heard |
| **Mapping** | Map screen showing nearby nodes |
| **Telemetry** | Battery voltage, temperature, humidity (with sensors) |
| **GPS** | NMEA GPS support (external module on UART) |
| **Channels** | Multiple named channels with AES256 encryption |
| **Power management** | Deep sleep, configurable TX power, solar support |
| **Bluetooth** | BLE pairing with Meshtastic Android / iOS app |
| **Wi-Fi** | Optional AP / client mode for web interface |
| **Admin** | Remote node administration |
| **Plugins/Modules** | Range test, serial bridge, external notification, MQTT gateway |
| **OTA** | Over-the-air firmware updates via Bluetooth |

### Joystick-navigable screens

| Screen | Contents |
|--------|----------|
| **Messages** | Received messages, compose new message |
| **Nodes** | List of mesh nodes with RSSI/SNR |
| **Map** | Overhead map of node positions |
| **Settings** | Channel, radio, power, display settings |
| **About** | Firmware version, device info |

---

## Joystick Navigation Controls

| Joystick Action | Meshtastic Action |
|-----------------|-------------------|
| **UP** | Scroll up / move selection up |
| **DOWN** | Scroll down / move selection down |
| **LEFT** | Previous screen |
| **RIGHT** | Next screen |
| **CENTER tap** | Confirm / Enter / Select |
| **CENTER long-press** (>= 800 ms) | Back / Cancel |

> **Tip:** The long-press threshold is adjustable. Set
> `JOYSTICK_LONG_PRESS_MS=500` in `platformio.ini` for faster response.

---

## Project Structure

```
.
├── platformio.ini                          # PlatformIO build configuration
├── README.md                               # This file
├── LICENSE
├── docs/
│   └── wiring.md                           # Detailed wiring diagrams
├── flash_tool/
│   ├── index.html                          # Web-based flash tool
│   └── firmware/                           # Place compiled .bin files here
│       ├── firmware-heltec-v3-joystick-2.7.26.bin
│       └── firmware-heltec-v4-joystick-2.7.26.bin
├── scripts/
│   └── pre_build.py                        # PlatformIO pre-build script
└── src/
    ├── main.cpp                            # Application entry point
    ├── input/
    │   ├── JoystickInput.h                 # Joystick driver header
    │   └── JoystickInput.cpp               # Joystick driver implementation
    └── variants/
        ├── heltec_v3_joystick/
        │   └── variant.h                   # V3 pin definitions
        └── heltec_v4_joystick/
            └── variant.h                   # V4 pin definitions
```

---

## Integration with Full Meshtastic Firmware

To use this joystick driver inside the **official Meshtastic firmware tree**:

1. Clone the official firmware:
   ```bash
   git clone https://github.com/meshtastic/firmware.git
   cd firmware
   git submodule update --init --recursive
   ```

2. Copy the joystick driver:
   ```bash
   cp -r /path/to/this/repo/src/input/JoystickInput.{h,cpp} src/input/
   ```

3. Copy the variant header:
   ```bash
   # For V3:
   cp -r /path/to/this/repo/src/variants/heltec_v3_joystick variants/heltec-v3-joystick/
   # For V4:
   cp -r /path/to/this/repo/src/variants/heltec_v4_joystick variants/heltec-v4-joystick/
   ```

4. In `src/main.cpp` of the Meshtastic firmware, add after `InputBroker` is
   instantiated:
   ```cpp
   #include "input/JoystickInput.h"

   JoystickInput joystick;   // uses defaults from platformio.ini build flags

   void setup() {
       // ... existing Meshtastic setup ...
       joystick.begin();
       joystick.onEvent([](const InputEvent &e) {
           // Forward to Meshtastic InputBroker
           inputBroker->receive(&e);
       });
   }

   void loop() {
       // ... existing Meshtastic loop ...
       joystick.update();
   }
   ```

5. Add the build flags to the Heltec environment in Meshtastic's `platformio.ini`:
   ```ini
   [env:heltec-v3]
   build_flags =
       ...existing flags...
       -DJOYSTICK_PIN_UP=1
       -DJOYSTICK_PIN_DOWN=2
       -DJOYSTICK_PIN_LEFT=4
       -DJOYSTICK_PIN_RIGHT=5
       -DJOYSTICK_PIN_CENTER=6
   ```

6. Build and upload as normal.

---

## Troubleshooting

### Board not detected by computer

- Try a different USB cable (data cable, not charge-only).
- Install the [CP2102 VCP driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
- On Linux: `sudo usermod -a -G dialout $USER` then re-login.

### Joystick direction not responding

- Check wiring: each direction → GPIO, common → GND.
- Verify the GPIO number matches `JOYSTICK_PIN_*` in `platformio.ini`.
- Open the serial monitor (`pio device monitor`) and push the joystick — you
  should see `[JOY] type=0x01` (or similar) logged.

### Display blank / scrambled

- Ensure `VEXT_CTRL` (GPIO 36) is driven LOW before I2C init.
- Check OLED RST (GPIO 21) is toggled HIGH during setup.
- Try a lower I2C clock in `Wire.setClock()`.

### Upload fails / wrong port

```bash
pio device list          # find your port
pio run -e heltec_v3_joystick --target upload --upload-port /dev/ttyUSB0
```

### Build errors

```bash
pio lib install          # install all library dependencies
pio run --target clean   # clean build cache
pio run -e heltec_v3_joystick
```

---

## FAQ

**Q: Can I change which GPIO pins the joystick uses?**
A: Yes. Edit the `JOYSTICK_PIN_*` values in `platformio.ini` under the
relevant `[env:...]` section. Avoid reserved pins (see `docs/wiring.md`).

**Q: Can I use an analog (thumbstick) joystick?**
A: This firmware targets passive digital 5-way switches. For an analog
thumbstick (two potentiometers) you need an ADC-based driver variant.

**Q: Will this work with Meshtastic Android/iOS apps?**
A: Yes — the firmware runs standard Meshtastic BLE/Wi-Fi pairing. The
joystick only affects the on-device OLED navigation.

**Q: What LoRa region should I configure?**
A: Set the region in Meshtastic settings to match your country:
`US` (915 MHz), `EU_868` (868 MHz), `EU_433` (433 MHz), etc.
Refer to https://meshtastic.org/docs/configuration/radio/lora/

**Q: Is AES encryption supported?**
A: Yes — Meshtastic uses AES256-CTR for all channel messages.

**Q: How do I update the firmware later?**
A: Re-run `pio run -e heltec_v3_joystick --target upload` or use the web
flash tool. For OTA, use the Meshtastic app over BLE.

---

## Contributing

Pull requests are welcome! For major changes, open an issue first.

1. Fork the repo and create a branch: `git checkout -b feature/my-feature`
2. Commit your changes: `git commit -m 'Add my feature'`
3. Push and open a PR.

Please follow the existing code style and add/update documentation as needed.

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Acknowledgements

- [Meshtastic Project](https://meshtastic.org) — open-source mesh radio firmware
- [Heltec Automation](https://heltec.org) — ESP32+LoRa hardware
- [Espressif Systems](https://espressif.com) — ESP32-S3 platform
- [PlatformIO](https://platformio.org) — embedded build system
