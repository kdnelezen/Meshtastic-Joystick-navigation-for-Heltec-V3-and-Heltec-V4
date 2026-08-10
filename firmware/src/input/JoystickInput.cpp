/**
 * JoystickInput.cpp
 *
 * Implementation of the 5-way passive joystick driver for Meshtastic on
 * Heltec V3 / V4 (ESP32-S3).
 */

#include "JoystickInput.h"

JoystickInput *joystickInput = nullptr;

// Provide a minimal notifyObservers implementation for standalone builds that
// do not link against the Meshtastic Observable/Observer framework.  When
// compiled inside the firmware tree, the real Observable from InputBroker.h
// takes precedence.
#if !defined(HAS_JOYSTICK) && defined(ARDUINO)
void Observable<const InputEvent *>::notifyObservers(const InputEvent *event)
{
    if (event && event->inputEvent != INPUT_BROKER_NONE) {
        Serial.print(F("[JOY] event="));
        Serial.println(event->inputEvent);
    }
}
#endif

JoystickInput::JoystickInput()
    : concurrency::OSThread("joystick"),
      _pinUp(JOYSTICK_PIN_UP),
      _pinDown(JOYSTICK_PIN_DOWN),
      _pinLeft(JOYSTICK_PIN_LEFT),
      _pinRight(JOYSTICK_PIN_RIGHT),
      _pinCenter(JOYSTICK_PIN_CENTER),
      _debounceMs(JOYSTICK_DEBOUNCE_MS),
      _longPressMs(JOYSTICK_LONG_PRESS_MS)
{
}

void JoystickInput::init()
{
    pinMode(_pinUp, INPUT_PULLUP);
    pinMode(_pinDown, INPUT_PULLUP);
    pinMode(_pinLeft, INPUT_PULLUP);
    pinMode(_pinRight, INPUT_PULLUP);
    pinMode(_pinCenter, INPUT_PULLUP);

    uint32_t now = millis();
    _stateUp.lastChange = now;
    _stateDown.lastChange = now;
    _stateLeft.lastChange = now;
    _stateRight.lastChange = now;
    _stateCenter.lastChange = now;

    LOG_DEBUG("Joystick GPIO initialized - UP:%d DOWN:%d LEFT:%d RIGHT:%d CENTER:%d", _pinUp, _pinDown, _pinLeft, _pinRight,
              _pinCenter);

    this->setInterval(50);
}

int32_t JoystickInput::runOnce()
{
    _updatePin(_stateUp, _pinUp, INPUT_BROKER_UP);
    _updatePin(_stateDown, _pinDown, INPUT_BROKER_DOWN);
    _updatePin(_stateLeft, _pinLeft, INPUT_BROKER_LEFT);
    _updatePin(_stateRight, _pinRight, INPUT_BROKER_RIGHT);
    _updateCenter();
    return 50;
}

void JoystickInput::_updatePin(PinState &ps, uint8_t pin, input_broker_event event)
{
    bool raw = (digitalRead(pin) == LOW);
    uint32_t now = millis();

    if (raw != ps.raw) {
        ps.raw = raw;
        ps.lastChange = now;
    }

    if ((now - ps.lastChange) < _debounceMs) {
        return;
    }

    if (raw != ps.stable) {
        ps.stable = raw;
        ps.reported = false;
    }

    if (ps.stable && !ps.reported) {
        ps.reported = true;
        _fire(event);
    }
}

void JoystickInput::_updateCenter()
{
    bool raw = (digitalRead(_pinCenter) == LOW);
    uint32_t now = millis();

    if (raw != _stateCenter.raw) {
        _stateCenter.raw = raw;
        _stateCenter.lastChange = now;
    }

    if ((now - _stateCenter.lastChange) < _debounceMs) {
        return;
    }

    if (raw != _stateCenter.stable) {
        if (!raw && _stateCenter.stable && !_stateCenter.longFired) {
            _fire(INPUT_BROKER_SELECT);
        }
        _stateCenter.stable = raw;
        _stateCenter.reported = false;
        _stateCenter.longFired = false;

        if (raw) {
            _stateCenter.pressTime = now;
        }
    }

    if (_stateCenter.stable && !_stateCenter.longFired) {
        if ((now - _stateCenter.pressTime) >= _longPressMs) {
            _stateCenter.longFired = true;
            _fire(INPUT_BROKER_BACK);
        }
    }
}

void JoystickInput::_fire(input_broker_event event)
{
    InputEvent e = {};
    e.source = "joystick";
    e.inputEvent = event;
    e.kbchar = 0;
    this->notifyObservers(&e);
}
