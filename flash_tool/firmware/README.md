# Firmware Binaries

Place compiled `.bin` firmware files here for use with the web flash tool.

## How to generate these files

### Option 1: Build with PlatformIO (recommended)

```bash
# From the repository root:
pio run -e heltec_v3_joystick
pio run -e heltec_v4_joystick
```

After a successful build, copy the output files:

```bash
# Heltec V3
cp .pio/build/heltec_v3_joystick/firmware.bin \
   flash_tool/firmware/firmware-heltec-v3-joystick-2.7.26.bin

# Heltec V4
cp .pio/build/heltec_v4_joystick/firmware.bin \
   flash_tool/firmware/firmware-heltec-v4-joystick-2.7.26.bin
```

### Option 2: Download pre-built release binaries

Check the [Releases page](https://github.com/kdnelezen/Meshtastic-Joystick-navigation-for-Heltec-V3-and-Heltec-V4/releases)
for pre-built `.bin` files attached to each release.

## Expected filenames (used by flash_tool/index.html)

| File | Board |
|------|-------|
| `firmware-heltec-v3-joystick-2.7.26.bin` | Heltec WiFi LoRa 32 V3 |
| `firmware-heltec-v4-joystick-2.7.26.bin` | Heltec WiFi LoRa 32 V4 |

> The filename version prefix (`2.7.26`) must match the `version` field in
> `flash_tool/index.html` → `FIRMWARE` object if you update it.
