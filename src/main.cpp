/**
 * main.cpp — Meshtastic + 5-way Joystick Navigation
 *
 * Supports: Heltec WiFi LoRa 32 V3 and V4 (ESP32-S3 + SX1262)
 *
 * Build with PlatformIO:
 *   pio run -e heltec_v3_joystick    # Heltec V3
 *   pio run -e heltec_v4_joystick    # Heltec V4
 *
 * Flash with PlatformIO:
 *   pio run -e heltec_v3_joystick --target upload
 *   pio run -e heltec_v4_joystick --target upload
 *
 * This file:
 *  1. Configures the OLED and Vext power rail.
 *  2. Initialises the JoystickInput driver.
 *  3. Routes joystick events into the local demo navigation state.
 *  4. Starts Meshtastic only when built with full firmware integration.
 *
 * Meshtastic firmware architecture overview
 * ──────────────────────────────────────────
 * The upstream Meshtastic firmware provides a rich set of modules
 * (messaging, telemetry, GPS, routing, power management …).
 * This project adds a thin hardware-abstraction layer on top so
 * that a passive 5-way joystick maps cleanly onto the existing
 * InputBroker event system that drives the on-screen menus.
 *
 * InputBroker event → OLED action:
 *   UP      → scroll up / move selection up
 *   DOWN    → scroll down / move selection down
 *   LEFT    → previous screen / back
 *   RIGHT   → next screen / forward
 *   CENTER  → confirm / select
 *   CENTER long-press → back / cancel
 */

#include <Arduino.h>
#include <Wire.h>

// Board variant header (selected by build flag in platformio.ini)
#if defined(HELTEC_V3)
#  include "variants/heltec_v3_joystick/variant.h"
#elif defined(HELTEC_V4)
#  include "variants/heltec_v4_joystick/variant.h"
#else
#  error "Unknown board: define HELTEC_V3 or HELTEC_V4 in platformio.ini"
#endif

// Joystick driver
#include "input/JoystickInput.h"

// ── OLED display ─────────────────────────────────────────────
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);

// ── Joystick ─────────────────────────────────────────────────
JoystickInput joystick(
    JOYSTICK_PIN_UP,
    JOYSTICK_PIN_DOWN,
    JOYSTICK_PIN_LEFT,
    JOYSTICK_PIN_RIGHT,
    JOYSTICK_PIN_CENTER,
    JOYSTICK_DEBOUNCE_MS,
    JOYSTICK_LONG_PRESS_MS
);

// ── Navigation state (demo / standalone mode) ─────────────────
// When linked into the full Meshtastic firmware tree the joystick
// events are consumed by InputBroker and the MeshService; this
// lightweight state machine only runs when building standalone.
namespace nav {
    static const char* SCREENS[] = {
        "Messages",
        "Nodes",
        "Map",
        "Settings",
        "About"
    };
    static const uint8_t NUM_SCREENS = sizeof(SCREENS) / sizeof(SCREENS[0]);
    static uint8_t currentScreen = 0;
    static uint8_t selectedItem  = 0;
    static bool    inMenu        = false;

    void nextScreen()  { currentScreen = (currentScreen + 1) % NUM_SCREENS; }
    void prevScreen()  { currentScreen = (currentScreen + NUM_SCREENS - 1) % NUM_SCREENS; }
    void moveUp()      { if (selectedItem > 0) selectedItem--; }
    void moveDown()    { if (selectedItem < 4) selectedItem++; }
    void select()      { inMenu = true; }
    void back()        { inMenu = false; selectedItem = 0; }
}

// ── OLED helpers ─────────────────────────────────────────────
static void initOled()
{
    // Power external peripherals (OLED is on Vext rail on V3/V4)
    pinMode(VEXT_CTRL, OUTPUT);
    digitalWrite(VEXT_CTRL, LOW);   // LOW = enable
    delay(100);

    Wire.begin(OLED_SDA, OLED_SCL);

    // Reset the OLED
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(50);
    digitalWrite(OLED_RST, HIGH);
    delay(50);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("[ERROR] SSD1306 OLED init failed"));
        while (true) { delay(1000); }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F(BOARD_NAME));
    display.println(F("Meshtastic"));
    display.println(F("Joystick Nav"));
    display.println(F(""));
    display.println(F("Initialising..."));
    display.display();
}

static void renderScreen()
{
    display.clearDisplay();

    // Header bar
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.fillRect(0, 0, OLED_WIDTH, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(2, 1);
    display.print(nav::SCREENS[nav::currentScreen]);

    display.setTextColor(SSD1306_WHITE);

    if (nav::inMenu) {
        // Show sub-items
        const char* items[] = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
        for (uint8_t i = 0; i < 5; i++) {
            uint8_t y = 14 + i * 10;
            if (i == nav::selectedItem) {
                display.fillRect(0, y - 1, OLED_WIDTH, 10, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            } else {
                display.setTextColor(SSD1306_WHITE);
            }
            display.setCursor(4, y);
            display.print(items[i]);
        }
    } else {
        // Navigation hint
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 16);
        display.println(F("LEFT/RIGHT: screen"));
        display.println(F("CENTER: enter menu"));
        display.println(F(""));
        display.print(F("Screen "));
        display.print(nav::currentScreen + 1);
        display.print(F("/"));
        display.print(nav::NUM_SCREENS);
    }

    // Footer: button hints
    display.drawLine(0, 55, OLED_WIDTH - 1, 55, SSD1306_WHITE);
    display.setCursor(0, 57);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print(nav::inMenu ? F("Long-press:back") : F("UP/DN scroll"));

    display.display();
}

// ── Joystick event handler ────────────────────────────────────
static void handleJoystickEvent(const InputEvent &event)
{
    Serial.print(F("[JOY] type=0x"));
    Serial.print(event.type, HEX);
    Serial.print(F(" long="));
    Serial.println(event.longPress ? F("yes") : F("no"));

    // Route events to navigation state
    switch (event.type) {
        case INPUT_BROKER_UP:
            nav::moveUp();
            break;
        case INPUT_BROKER_DOWN:
            nav::moveDown();
            break;
        case INPUT_BROKER_LEFT:
            nav::prevScreen();
            nav::back();
            break;
        case INPUT_BROKER_RIGHT:
            nav::nextScreen();
            nav::back();
            break;
        case INPUT_BROKER_SELECT:
            nav::select();
            break;
        case INPUT_BROKER_BACK:
            nav::back();
            break;
        default:
            break;
    }

    renderScreen();
}

// ── Arduino entry points ──────────────────────────────────────
void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F(" Meshtastic Joystick Navigation"));
    Serial.println(F(" " BOARD_NAME));
    Serial.println(F("=============================================="));

    initOled();

    joystick.begin();
    joystick.onEvent(handleJoystickEvent);

    Serial.println(F("[OK] Joystick initialised"));
    Serial.print(F("  UP    → GPIO ")); Serial.println(JOYSTICK_PIN_UP);
    Serial.print(F("  DOWN  → GPIO ")); Serial.println(JOYSTICK_PIN_DOWN);
    Serial.print(F("  LEFT  → GPIO ")); Serial.println(JOYSTICK_PIN_LEFT);
    Serial.print(F("  RIGHT → GPIO ")); Serial.println(JOYSTICK_PIN_RIGHT);
    Serial.print(F("  CTR   → GPIO ")); Serial.println(JOYSTICK_PIN_CENTER);

    // ── Meshtastic mesh init ───────────────────────────────────
    // When this file is compiled as part of the full Meshtastic
    // firmware tree, replace the section below with the actual
    // Meshtastic::begin() call or use the standard main.cpp from
    // the upstream repo and only keep the joystick bootstrap.
    //
    // For standalone/demo builds the mesh stack is stubbed out.
#ifdef MESHTASTIC_FIRMWARE_INCLUDED
    // Full Meshtastic initialisation
    extern void meshtasticBegin();
    meshtasticBegin();
#else
    Serial.println(F("[INFO] Running in standalone demo mode"));
    Serial.println(F("       (link against Meshtastic firmware for full mesh)"));
#endif

    delay(500);
    renderScreen();
    Serial.println(F("[OK] Setup complete — entering main loop"));
}

void loop()
{
    // Poll the joystick (debounce + long-press handled inside)
    joystick.update();

#ifdef MESHTASTIC_FIRMWARE_INCLUDED
    // Hand control to the Meshtastic scheduler
    extern void meshtasticLoop();
    meshtasticLoop();
#else
    delay(10);  // yield in demo/standalone mode
#endif
}
