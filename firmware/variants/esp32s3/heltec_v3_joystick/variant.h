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
// HAS_TRACKBALL selects the upstream TrackballInterruptBase driver.  Despite
// the name, that driver is a generic 5-way directional input source that
// feeds the InputBroker UP/DOWN/LEFT/RIGHT/SELECT/SELECT_LONG events.
#define HAS_TRACKBALL 1

// Joystick GPIOs (active LOW, internal pull-up enabled by driver).
#define TB_UP    1
#define TB_DOWN  2
#define TB_LEFT  4
#define TB_RIGHT 5
#define TB_PRESS 6

// Interrupt edge: FALLING because the switch pulls the line to GND.
#define TB_DIRECTION FALLING

// Number of pulses required before an event fires.  0 = fire on first pulse.
#define TB_THRESHOLD 0
