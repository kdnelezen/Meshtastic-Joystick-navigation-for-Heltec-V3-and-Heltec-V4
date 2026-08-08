# Joystick Wiring Diagrams

## Overview

A passive 5-way navigation joystick (e.g. RKJXK series, SKRHAAE, or generic
PSP-style stick) connects directly to the free GPIO header pins on the Heltec
V3 / V4 board.  No external resistors are needed because the firmware enables
internal pull-ups on every joystick pin.  Each direction switch connects
between its GPIO and GND; pressing the stick pulls the line LOW.

---

## Pin Mapping — Heltec V3 and V4

Both boards share the same free-GPIO assignments:

| Joystick Function | GPIO | Heltec Header | Notes                          |
|-------------------|------|---------------|--------------------------------|
| UP                |  1   | IO1           | Active-LOW, internal pull-up   |
| DOWN              |  2   | IO2           | Active-LOW, internal pull-up   |
| LEFT              |  4   | IO4           | Active-LOW, internal pull-up   |
| RIGHT             |  5   | IO5           | Active-LOW, internal pull-up   |
| CENTER / CLICK    |  6   | IO6           | Active-LOW; long-press = BACK  |
| GND (common)      | GND  | GND           | Joystick common pin            |
| VCC (optional)    | 3V3  | 3V3           | Only if the module needs power |

> **Note:** GPIO 1, 2, 4, 5, 6 are confirmed free on both V3 and V4.  Do
> **not** use GPIO 0 (boot button), 8–14 (LoRa SPI), 17–18 (OLED I2C),
> 21 (OLED RST), 35 (LED), or 36 (Vext control).

---

## ASCII Wiring Diagram

```
  HELTEC V3 / V4                      5-WAY JOYSTICK MODULE
  ┌─────────────────┐                 ┌─────────────────────┐
  │                 │                 │   [UP]              │
  │  3V3  ──────────┼─────────────────┼── VCC (if needed)   │
  │  GND  ──────────┼─────────────────┼── COM / GND         │
  │  IO1  ──────────┼─────────────────┼── UP                │
  │  IO2  ──────────┼─────────────────┼── DOWN              │
  │  IO4  ──────────┼─────────────────┼── LEFT              │
  │  IO5  ──────────┼─────────────────┼── RIGHT             │
  │  IO6  ──────────┼─────────────────┼── CENTER / CLICK    │
  │                 │                 └─────────────────────┘
  │  (reserved)     │
  │  IO0  = User button (boot)
  │  IO8–IO14 = LoRa SPI (SX1262)
  │  IO17/IO18 = OLED I2C
  │  IO21 = OLED RST
  │  IO35 = LED
  │  IO36 = Vext control
  └─────────────────┘
```

---

## Schematic Logic (per direction)

```
  GPIO (INPUT_PULLUP)
       │
       ├─── 10 kΩ (internal pull-up, enabled by firmware)
       │
      [SW]  ← joystick direction switch
       │
      GND
```

When the stick is moved to a direction, SW closes → GPIO reads LOW → firmware
fires the corresponding InputBroker event.

---

## Board Header Locations

### Heltec V3

```
Left header (top → bottom):
  GND, 3V3, 5V, IO26, IO33, IO32, IO13, IO12, IO11, IO10, IO9, IO8

Right header (top → bottom):
  GND, RST, IO0, IO36, IO35, IO34, IO21, IO20, IO19, IO18, IO17
  IO16, IO15, IO7, IO6, IO5, IO4, IO3, IO2, IO1
```

> **IO1, IO2, IO4, IO5, IO6** are on the **right header**, near the bottom.

### Heltec V4

The V4 uses an identical header layout to the V3.  The same GPIO numbers
apply.  Refer to the official Heltec V4 pinout diagram for exact physical
positions: https://docs.heltec.org

---

## Recommended Joystick Modules

| Module            | Link type    | Notes                               |
|-------------------|--------------|-------------------------------------|
| RKJXK series      | SMD / THT    | Industry-standard 5-way nav switch  |
| SKRHAAE series    | SMD          | Alps 5-way, very low travel         |
| PSP joystick hat  | Breakout PCB | Includes all 5 lines + common       |
| Grove 5-way joystick | Grove    | Has pull-ups on board; skip VCC wire|

All modules work with this firmware; the only requirement is that each
direction connects between a GPIO and GND (active-LOW logic).

---

## Long-Press Behavior

| Action               | Event fired       | Default behavior       |
|----------------------|-------------------|------------------------|
| CENTER tap (< 800 ms)| INPUT_BROKER_SELECT | Confirm / enter menu |
| CENTER hold (≥ 800 ms)| INPUT_BROKER_BACK | Back / cancel          |

The long-press threshold is configurable via `JOYSTICK_LONG_PRESS_MS` in
`platformio.ini` (default: 800 ms).

---

## Troubleshooting

| Symptom                          | Likely cause                     | Fix                            |
|----------------------------------|----------------------------------|--------------------------------|
| Direction not responding         | Wrong GPIO or loose wire         | Check wiring against table above |
| Direction fires continuously     | GPIO floating (no pull-up)       | Ensure firmware sets INPUT_PULLUP; check GND connection |
| CENTER doesn't long-press        | Debounce too long                | Reduce `JOYSTICK_LONG_PRESS_MS` |
| Multiple directions fire at once | Crosstalk / short circuit        | Inspect soldering; ensure common wire is only GND |
| OLED blank after wiring          | Vext not powered                 | Check IO36 is driven LOW in firmware setup |
