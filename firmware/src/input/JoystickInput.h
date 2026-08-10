/**
 * JoystickInput.h
 *
 * Meshtastic InputBroker-compatible driver for a passive 5-way navigation
 * joystick (UP / DOWN / LEFT / RIGHT / CENTER).
 *
 * The joystick common pin connects to GND; each direction pin connects to
 * an ESP32-S3 GPIO configured as INPUT_PULLUP.  A press pulls the line LOW.
 *
 * This driver is activated by defining HAS_JOYSTICK in the board variant and
 * polls the GPIOs from a lightweight OSThread.  It emits standard InputBroker
 * events:
 *   UP     → INPUT_BROKER_UP
 *   DOWN   → INPUT_BROKER_DOWN
 *   LEFT   → INPUT_BROKER_LEFT
 *   RIGHT  → INPUT_BROKER_RIGHT
 *   CENTER → INPUT_BROKER_SELECT
 *   CENTER long-press → INPUT_BROKER_BACK
 */

#pragma once

#include "InputBroker.h"
#include "concurrency/OSThread.h"

#ifndef JOYSTICK_PIN_UP
#define JOYSTICK_PIN_UP 1
#endif
#ifndef JOYSTICK_PIN_DOWN
#define JOYSTICK_PIN_DOWN 2
#endif
#ifndef JOYSTICK_PIN_LEFT
#define JOYSTICK_PIN_LEFT 4
#endif
#ifndef JOYSTICK_PIN_RIGHT
#define JOYSTICK_PIN_RIGHT 5
#endif
#ifndef JOYSTICK_PIN_CENTER
#define JOYSTICK_PIN_CENTER 6
#endif

#ifndef JOYSTICK_DEBOUNCE_MS
#define JOYSTICK_DEBOUNCE_MS 50
#endif
#ifndef JOYSTICK_LONG_PRESS_MS
#define JOYSTICK_LONG_PRESS_MS 800
#endif

class JoystickInput : public Observable<const InputEvent *>, public concurrency::OSThread {
  public:
    JoystickInput();
    void init();
    int32_t runOnce() override;

  private:
    uint8_t _pinUp, _pinDown, _pinLeft, _pinRight, _pinCenter;
    uint32_t _debounceMs;
    uint32_t _longPressMs;

    struct PinState {
        bool raw = false;
        bool stable = false;
        bool reported = false;
        uint32_t lastChange = 0;
    };

    PinState _stateUp, _stateDown, _stateLeft, _stateRight;

    struct CenterState : PinState {
        bool longFired = false;
        uint32_t pressTime = 0;
    } _stateCenter;

    void _updatePin(PinState &ps, uint8_t pin, input_broker_event event);
    void _updateCenter();
    void _fire(input_broker_event event);
};

extern JoystickInput *joystickInput;
