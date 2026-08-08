/**
 * JoystickInput.cpp
 *
 * Implementation of the 5-way passive joystick driver for Meshtastic on
 * Heltec V3 / V4 (ESP32-S3).
 *
 * See JoystickInput.h for full API documentation.
 */

#include "JoystickInput.h"

// ── Construction ──────────────────────────────────────────────
JoystickInput::JoystickInput(
    uint8_t  pinUp,
    uint8_t  pinDown,
    uint8_t  pinLeft,
    uint8_t  pinRight,
    uint8_t  pinCenter,
    uint32_t debounceMs,
    uint32_t longPressMs
) :
    _pinUp(pinUp),
    _pinDown(pinDown),
    _pinLeft(pinLeft),
    _pinRight(pinRight),
    _pinCenter(pinCenter),
    _debounceMs(debounceMs),
    _longPressMs(longPressMs)
{}


// ── begin ─────────────────────────────────────────────────────
void JoystickInput::begin()
{
    // All joystick lines use internal pull-ups; pressed = LOW
    pinMode(_pinUp,     INPUT_PULLUP);
    pinMode(_pinDown,   INPUT_PULLUP);
    pinMode(_pinLeft,   INPUT_PULLUP);
    pinMode(_pinRight,  INPUT_PULLUP);
    pinMode(_pinCenter, INPUT_PULLUP);

    // Seed timestamps so first read doesn't look like a bounce
    uint32_t now = millis();
    _stateUp.lastChange     = now;
    _stateDown.lastChange   = now;
    _stateLeft.lastChange   = now;
    _stateRight.lastChange  = now;
    _stateCenter.lastChange = now;
}


// ── onEvent ───────────────────────────────────────────────────
void JoystickInput::onEvent(InputBrokerHandler handler)
{
    _handler = handler;
}


// ── isAnyPressed ──────────────────────────────────────────────
bool JoystickInput::isAnyPressed() const
{
    return (digitalRead(_pinUp)     == LOW ||
            digitalRead(_pinDown)   == LOW ||
            digitalRead(_pinLeft)   == LOW ||
            digitalRead(_pinRight)  == LOW ||
            digitalRead(_pinCenter) == LOW);
}


// ── update ────────────────────────────────────────────────────
void JoystickInput::update()
{
    _updatePin(_stateUp,    _pinUp,    INPUT_BROKER_UP);
    _updatePin(_stateDown,  _pinDown,  INPUT_BROKER_DOWN);
    _updatePin(_stateLeft,  _pinLeft,  INPUT_BROKER_LEFT);
    _updatePin(_stateRight, _pinRight, INPUT_BROKER_RIGHT);
    _updateCenter();
}


// ── _updatePin  (directional, no long-press) ─────────────────
void JoystickInput::_updatePin(PinState &ps, uint8_t pin, uint8_t eventType)
{
    bool raw = (digitalRead(pin) == LOW);   // active-LOW
    uint32_t now = millis();

    // Detect transition
    if (raw != ps.raw) {
        ps.raw        = raw;
        ps.lastChange = now;
    }

    // Wait for debounce to settle
    if ((now - ps.lastChange) < _debounceMs) {
        return;
    }

    if (raw != ps.stable) {
        ps.stable   = raw;
        ps.reported = false;
    }

    // Fire one event per press (on leading edge)
    if (ps.stable && !ps.reported) {
        ps.reported = true;
        _fire(eventType);
    }
}


// ── _updateCenter  (with long-press detection) ────────────────
void JoystickInput::_updateCenter()
{
    bool raw = (digitalRead(_pinCenter) == LOW);
    uint32_t now = millis();

    // Detect transition
    if (raw != _stateCenter.raw) {
        _stateCenter.raw        = raw;
        _stateCenter.lastChange = now;
    }

    // Wait for debounce
    if ((now - _stateCenter.lastChange) < _debounceMs) {
        return;
    }

    if (raw != _stateCenter.stable) {
        // If button released before long-press fired → short press
        if (!raw && _stateCenter.stable && !_stateCenter.longFired) {
            _fire(INPUT_BROKER_SELECT, false);
        }
        _stateCenter.stable    = raw;
        _stateCenter.reported  = false;
        _stateCenter.longFired = false;

        if (raw) {
            _stateCenter.pressTime = now;   // record press start
        }
    }

    // Long-press detection (while still held)
    if (_stateCenter.stable && !_stateCenter.longFired) {
        if ((now - _stateCenter.pressTime) >= _longPressMs) {
            _stateCenter.longFired = true;
            _fire(INPUT_BROKER_BACK, true);
        }
    }
}


// ── _fire ─────────────────────────────────────────────────────
void JoystickInput::_fire(uint8_t type, bool longPress)
{
    if (_handler == nullptr) return;

    InputEvent ev;
    ev.type      = type;
    ev.longPress = longPress;
    ev.timestamp = millis();

    _handler(ev);
}
