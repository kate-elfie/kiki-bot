# Wiring

Board: ESP32-S3 DevKitC-1 **N16R8**. Pin numbers below are the `GPIO` numbers printed next to each pin on the board. The same map is in [`include/pins.h`](../include/pins.h).

## Before you start

- Unplug USB while you move wires.
- Use 2 breadboards side by side. The board sits across the gap, so both rows of pins stay free.
- Make 2 power rails: **3V3 rail** from the board's `3V3` pin, **5V rail** from the board's `5V` pin. Join all `GND` together.
- Plug the USB cable into the port labelled **COM** to see Serial Monitor output.

## Step 1: eyes + touch

### Round display GC9A01

| Display pin | Connect to | Note |
|---|---|---|
| VCC | 3V3 rail | |
| GND | GND | |
| SCL | GPIO 12 | SPI clock. The label says SCL but this is **not I2C** |
| SDA | GPIO 11 | SPI data. The label says SDA but this is **not I2C** |
| DC | GPIO 9 | |
| CS | GPIO 10 | |
| RST | GPIO 14 | |

### Touch pad (no sensor needed)

The ESP32-S3 has touch sensing built in on GPIO 1–14.

| Part | Connect to |
|---|---|
| One end of a jumper wire | GPIO 8 |
| Other end | A piece of foil, a coin, or copper tape: Kiki's "head" |

- Only 1 wire. No VCC, no GND.
- Keep the wire short (under 20 cm). Long wires make it touchy.
- Do not touch the pad while Kiki boots. She measures the idle level then.
- Too sensitive or not sensitive enough? Type `touch` in Serial Monitor to see the readings, then change `kTouchOn` in `src/main.cpp`.

## Step 2: voice (code not written yet)

### Mic INMP441

| Mic pin | Connect to | Note |
|---|---|---|
| VDD | 3V3 rail | **3.3V only.** 5V can kill it |
| GND | GND | |
| SCK | GPIO 4 | |
| WS | GPIO 5 | |
| SD | GPIO 6 | |
| L/R | GND | GND = left channel |

### Amp MAX98357A + speaker

| Amp pin | Connect to | Note |
|---|---|---|
| VIN | 5V rail | 5V gives full volume. 3.3V works but is much quieter |
| GND | GND | |
| BCLK | GPIO 15 | |
| LRC | GPIO 16 | |
| DIN | GPIO 7 | |
| GAIN | leave empty | 9 dB by default |
| SD | leave empty | Amp is on by default |
| Speaker + / − | speaker wires | **Never connect Speaker − to GND** |

## Step 3: moving head (code not written yet)

### 2 servos SG90

| Wire colour | Connect to |
|---|---|
| Brown | GND |
| Red | 5V rail (**not** 3V3) |
| Orange, pan servo (left/right) | GPIO 17 |
| Orange, tilt servo (up/down) | GPIO 18 |

If the board restarts when the servos move, USB cannot give enough current. Add a 470–1000 µF capacitor between the 5V rail and GND, or power the servos from a separate 5V supply (keep GND shared).

## Extra sensors (code not written yet)

### Motion sensor MPU6050 (GY-521)

Lets Kiki feel being shaken or picked up.

| Sensor pin | Connect to | Note |
|---|---|---|
| VCC | 3V3 rail | |
| GND | GND | |
| SDA | GPIO 1 | Real I2C this time |
| SCL | GPIO 2 | Real I2C this time |
| XDA, XCL, AD0, INT | leave empty | |

### IR obstacle sensor

Lets Kiki see a hand waving in front of her.

| Sensor pin | Connect to | Note |
|---|---|---|
| VCC | 3V3 rail | |
| GND | GND | |
| OUT (or DO) | GPIO 21 | LOW when something is in front |

Turn the small screw on the board to set the distance. The second LED lights up when it sees something.

## Pins to avoid

| GPIO | Why |
|---|---|
| 35, 36, 37 | Used by the 8MB PSRAM on N16R8 boards |
| 0, 3, 45, 46 | Boot pins, the board may not start |
| 19, 20 | Native USB port |
| 43, 44 | COM port (Serial Monitor) |
| 26–32 | Flash memory |
