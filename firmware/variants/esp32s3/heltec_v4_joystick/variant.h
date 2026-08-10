/**
 * variant.h — Heltec WiFi LoRa 32 V4 + 5-way joystick
 *
 * This variant extends the upstream Heltec V4 configuration and adds the
 * pin definitions required for the Meshtastic Joystick Navigation mod.
 *
 * The V4 has more occupied pins than the V3 (FEM control, GPS, second I2C).
 * The defaults below avoid:
 *   - LoRa radio pins (8–14)
 *   - FEM control pins (2, 5, 7, 46)
 *   - VEXT enable (36), ADC control (37), battery ADC (1)
 *   - User/boot button (0)
 *   - Primary OLED I2C (17, 18) and secondary I2C (3, 4/47, 48)
 *   - GPS block (34, 38–42)
 *
 * Default wiring:
 *   UP     → GPIO 15
 *   DOWN   → GPIO 16
 *   LEFT   → GPIO 19
 *   RIGHT  → GPIO 20
 *   CENTER → GPIO 6
 *
 * Verify against your specific V4 board silkscreen before wiring.
 */

#pragma once

// Pull in the upstream Heltec V4 pin definitions first.
#include "../heltec_v4/variant.h"

// Override the board name for logs and the about screen.
#ifdef BOARD_NAME
#undef BOARD_NAME
#endif
#define BOARD_NAME "Heltec V4 + Joystick"

// ── 5-way joystick ───────────────────────────────────────────
#define HAS_TRACKBALL 1

// Joystick GPIOs (active LOW, internal pull-up enabled by driver).
#define TB_UP    15
#define TB_DOWN  16
#define TB_LEFT  19
#define TB_RIGHT 20
#define TB_PRESS 6

// Interrupt edge: FALLING because the switch pulls the line to GND.
#define TB_DIRECTION FALLING

// Number of pulses required before an event fires.  0 = fire on first pulse.
#define TB_THRESHOLD 0
