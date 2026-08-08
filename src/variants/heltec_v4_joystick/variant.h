/**
 * variant.h — Heltec WiFi LoRa 32 V4 + 5-way Joystick
 *
 * Board:  Heltec WiFi LoRa 32 V4 (ESP32-S3R2)
 * Radio:  Semtech SX1262
 * Display: 0.96" SSD1306 OLED (I2C)
 *
 * 5-way joystick wiring (recommended free GPIOs):
 *
 *   Joystick pin  →  Heltec V4 header pin  →  GPIO
 *   ─────────────────────────────────────────────────
 *   UP             →  IO1                  →  GPIO 1
 *   DOWN           →  IO2                  →  GPIO 2
 *   LEFT           →  IO4                  →  GPIO 4
 *   RIGHT          →  IO5                  →  GPIO 5
 *   CENTER/CLICK   →  IO6                  →  GPIO 6
 *   GND            →  GND                  →  GND
 *   VCC (3.3 V)    →  3V3                  →  3.3 V
 *
 * V4 note: lower sleep current (~140 µA vs ~180 µA on V3),
 * dedicated GPS / solar connectors, improved antenna matching.
 * All joystick inputs use internal pull-ups; active-LOW logic.
 */

#pragma once

// ── Heltec V4 reserved / on-board pins (same mapping as V3) ──
#define LORA_SCK          9
#define LORA_MISO         11
#define LORA_MOSI         10
#define LORA_CS           8
#define LORA_RST          12
#define LORA_DIO1         14
#define LORA_BUSY         13

#define OLED_SDA          17
#define OLED_SCL          18
#define OLED_RST          21

#define VEXT_CTRL         36     // Drive LOW to enable 3.3 V external supply
#define LED_PIN           35
#define USER_BTN          0      // Boot / user button

// ── 5-way joystick ───────────────────────────────────────────
#ifndef JOYSTICK_PIN_UP
#  define JOYSTICK_PIN_UP      1
#endif
#ifndef JOYSTICK_PIN_DOWN
#  define JOYSTICK_PIN_DOWN    2
#endif
#ifndef JOYSTICK_PIN_LEFT
#  define JOYSTICK_PIN_LEFT    4
#endif
#ifndef JOYSTICK_PIN_RIGHT
#  define JOYSTICK_PIN_RIGHT   5
#endif
#ifndef JOYSTICK_PIN_CENTER
#  define JOYSTICK_PIN_CENTER  6
#endif

#ifndef JOYSTICK_DEBOUNCE_MS
#  define JOYSTICK_DEBOUNCE_MS    50
#endif
#ifndef JOYSTICK_LONG_PRESS_MS
#  define JOYSTICK_LONG_PRESS_MS  800
#endif

// ── Convenience macro ─────────────────────────────────────────
#define BOARD_NAME "Heltec V4 + Joystick"
