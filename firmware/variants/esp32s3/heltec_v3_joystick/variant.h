/**
 * variant.h — Heltec WiFi LoRa 32 V3 + 5-way joystick
 *
 * This variant extends the upstream Heltec V3 configuration and adds the
 * pin definitions required for the Meshtastic Joystick Navigation mod.
 *
 * The 5-way joystick is wired as a passive switch matrix: one common pin to
 * GND and one GPIO per direction.  The firmware enables internal pull-ups,
 * so pressing a direction pulls the line LOW.
 *
 * Default wiring:
 *   UP     → GPIO 1
 *   DOWN   → GPIO 2
 *   LEFT   → GPIO 4
 *   RIGHT  → GPIO 5
 *   CENTER → GPIO 6
 *
 * These defaults match the silkscreened IO1–IO6 header on the Heltec V3.
 */

#pragma once

// Pull in the upstream Heltec V3 pin definitions first.
#include "../heltec_v3/variant.h"

// Override the board name for logs and the about screen.
#ifdef BOARD_NAME
#undef BOARD_NAME
#endif
#define BOARD_NAME "Heltec V3 + Joystick"

// ── 5-way joystick ───────────────────────────────────────────
// Activate the custom JoystickInput driver.
#define HAS_JOYSTICK 1

// Joystick GPIOs (active LOW, internal pull-up enabled by driver).
#define JOYSTICK_PIN_UP    1
#define JOYSTICK_PIN_DOWN  2
#define JOYSTICK_PIN_LEFT  4
#define JOYSTICK_PIN_RIGHT 5
#define JOYSTICK_PIN_CENTER 6

// Debounce and long-press timing.
#define JOYSTICK_DEBOUNCE_MS   50
#define JOYSTICK_LONG_PRESS_MS 800
