/**
 * StandaloneJoystick.ino
 *
 * Minimal example that exercises the JoystickInput driver without the full
 * Meshtastic firmware tree.  Useful for verifying wiring and debounce timing.
 *
 * Upload this to a Heltec V3 or V4 using the Arduino IDE or PlatformIO with
 * the matching variant header.  Open the serial monitor at 115200 baud and
 * push the joystick - you should see events printed for each direction.
 */

#include <Arduino.h>

// Use the same pin definitions the firmware variant uses.
#include "variants/esp32s3/heltec_v3_joystick/variant.h"
#include "input/JoystickInput.h"

JoystickInput joystick;

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {
        delay(10);
    }
    Serial.println();
    Serial.println(F("Standalone Joystick Test"));
    Serial.println(F("Push the 5-way joystick to see events."));

    joystick.init();
}

void loop()
{
    joystick.runOnce();
    delay(10);
}
