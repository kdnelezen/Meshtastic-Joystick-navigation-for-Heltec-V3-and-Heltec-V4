/**
 * JoystickInput.h
 *
 * Meshtastic InputBroker-compatible driver for a passive 5-way navigation
 * joystick (UP / DOWN / LEFT / RIGHT / CENTER).
 *
 * The joystick common pin connects to GND; each direction pin connects to
 * an ESP32-S3 GPIO configured as INPUT_PULLUP.  A press pulls the line LOW.
 *
 * Event mapping to Meshtastic InputBroker:
 *   UP     → INPUT_BROKER_UP       (scroll / navigate up)
 *   DOWN   → INPUT_BROKER_DOWN     (scroll / navigate down)
 *   LEFT   → INPUT_BROKER_LEFT     (previous screen / back)
 *   RIGHT  → INPUT_BROKER_RIGHT    (next screen / forward)
 *   CENTER → INPUT_BROKER_SELECT   (confirm / enter)
 *   CENTER (long) → INPUT_BROKER_BACK  (cancel / back one level)
 */

#pragma once

#include <Arduino.h>

// ── InputBroker event type identifiers ───────────────────────
// These mirror the values used in the Meshtastic firmware.
// If compiling inside the full Meshtastic tree, the real
// InputBroker.h definitions take precedence via include guards.
#ifndef INPUT_BROKER_UP
#  define INPUT_BROKER_UP      0x01
#  define INPUT_BROKER_DOWN    0x02
#  define INPUT_BROKER_LEFT    0x03
#  define INPUT_BROKER_RIGHT   0x04
#  define INPUT_BROKER_SELECT  0x05
#  define INPUT_BROKER_BACK    0x06
#  define INPUT_BROKER_NONE    0x00
#endif

// ── Pin defaults (can be overridden in platformio.ini) ───────
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

// ── Forward declaration ───────────────────────────────────────
class JoystickInput;

/**
 * InputEvent
 * Minimal event structure compatible with Meshtastic's InputBroker.
 * When building inside the full firmware tree the real struct is used.
 */
#ifndef INPUT_EVENT_DEFINED
#define INPUT_EVENT_DEFINED
struct InputEvent {
    uint8_t  type     = INPUT_BROKER_NONE;  ///< Event type constant
    bool     longPress = false;             ///< True when center is held
    uint32_t timestamp = 0;                 ///< millis() at event creation
};
#endif

/**
 * InputBrokerHandler
 * Minimal callback interface.  In the full Meshtastic tree the real
 * InputBroker replaces this with its own dispatch mechanism.
 */
#ifndef INPUT_BROKER_HANDLER_DEFINED
#define INPUT_BROKER_HANDLER_DEFINED
typedef void (*InputBrokerHandler)(const InputEvent &event);
#endif


/**
 * JoystickInput
 *
 * Polls the five joystick GPIO lines, applies debouncing, detects
 * long-press on the center button, and fires InputEvent callbacks.
 *
 * Usage (standalone / outside full Meshtastic tree):
 * @code
 *   JoystickInput joystick;
 *
 *   void setup() {
 *       joystick.begin();
 *       joystick.onEvent([](const InputEvent &e) {
 *           // handle e.type
 *       });
 *   }
 *
 *   void loop() {
 *       joystick.update();   // call every loop iteration
 *   }
 * @endcode
 *
 * Usage (inside Meshtastic tree):
 *   Instantiate once in main.cpp; call begin() in setup(), and let the
 *   existing InputBroker scheduler call update() via a registered task.
 */
class JoystickInput {
public:
    // ── Construction ─────────────────────────────────────────
    JoystickInput(
        uint8_t pinUp     = JOYSTICK_PIN_UP,
        uint8_t pinDown   = JOYSTICK_PIN_DOWN,
        uint8_t pinLeft   = JOYSTICK_PIN_LEFT,
        uint8_t pinRight  = JOYSTICK_PIN_RIGHT,
        uint8_t pinCenter = JOYSTICK_PIN_CENTER,
        uint32_t debounceMs   = JOYSTICK_DEBOUNCE_MS,
        uint32_t longPressMs  = JOYSTICK_LONG_PRESS_MS
    );

    // ── Lifecycle ─────────────────────────────────────────────
    void begin();
    void update();              ///< Must be called from loop() / scheduler

    // ── Callback registration ─────────────────────────────────
    void onEvent(InputBrokerHandler handler);

    // ── Diagnostic ───────────────────────────────────────────
    bool isAnyPressed() const;

private:
    // Pin numbers
    uint8_t  _pinUp, _pinDown, _pinLeft, _pinRight, _pinCenter;

    // Timing constants
    uint32_t _debounceMs;
    uint32_t _longPressMs;

    // Debounce state per direction
    struct PinState {
        bool     raw       = false;
        bool     stable    = false;
        bool     reported  = false;   // have we fired the press event?
        uint32_t lastChange = 0;
    };

    PinState _stateUp, _stateDown, _stateLeft, _stateRight;

    // Center button needs long-press detection too
    struct CenterState : PinState {
        bool     longFired  = false;
        uint32_t pressTime  = 0;
    } _stateCenter;

    // Registered callback
    InputBrokerHandler _handler = nullptr;

    // Helpers
    void _updatePin(PinState &ps, uint8_t pin, uint8_t eventType);
    void _updateCenter();
    void _fire(uint8_t type, bool longPress = false);
};
