# kiki-bot

Kiki is a small desk robot built from scratch on an ESP32-S3. She shows emotions with two eyes on a round screen, reacts to touch, and later listens, talks, and moves her head.

## Steps

| Step | What Kiki can do | Status |
|---|---|---|
| 1 | 7 moods on the round screen, blinks, looks around, happy when touched | Code ready, waiting for parts |
| 2 | Hears you (mic), talks back with AI (speaker) | Not started |
| 3 | Moves her head with 2 servos | Not started |

## Parts (~500k VND)

| Part | Job | Price |
|---|---|---|
| ESP32-S3 DevKitC-1 **N16R8**, soldered pins | Brain | ~130–150k |
| GC9A01 1.28" round display, 240×240 | Face | ~106k |
| INMP441 I2S mic, soldered pins | Ears | 55k |
| MAX98357A I2S amp + boxed speaker 4–8 Ω | Mouth | ~90k |
| 2× SG90 servo | Neck | ~50k |
| TTP223 touch sensor | Touch | ~5k |
| 2 breadboards + jumper wires | Wiring | ~45k |

How to connect everything: [docs/wiring.md](docs/wiring.md).

## Build and flash

1. Install [VS Code](https://code.visualstudio.com/) and the **PlatformIO IDE** extension.
2. Open this folder in VS Code.
3. Plug the board into your computer using the USB port labelled **COM**.
4. Click **Upload** (→ arrow) in the PlatformIO bar at the bottom.
5. Click **Serial Monitor** (plug icon) to talk to Kiki.

Command line instead: `pio run -t upload && pio device monitor`.

If upload fails: hold **BOOT**, press and release **RST**, release **BOOT**, then upload again.

## Talk to Kiki (Serial Monitor)

Kiki starts in demo mode: she shows each mood for 3 seconds.

| Type | Kiki does |
|---|---|
| `neutral` `happy` `sad` `angry` `surprised` `sleepy` `love` | Shows that mood, stops the demo |
| `demo` | Turns the demo on or off |

Touch: a short touch makes her happy, holding 1.5 s turns her eyes into hearts. After 4 s she goes back to her mood.

## Secrets

This repo is public. WiFi password and API keys go in `include/secrets.h`, which git ignores. Copy `include/secrets.example.h` to start. Never commit real keys.

## Troubleshooting

| You see | Fix |
|---|---|
| Black screen | Check SCL → GPIO 12 and SDA → GPIO 11 (it is SPI, not I2C), then VCC on 3V3 |
| Colours look negative | In `include/display.h`, set `cfg.invert = false` |
| Nothing in Serial Monitor | Use the **COM** USB port, speed 115200 |
| Board restarts when servos move | See the servo note in [docs/wiring.md](docs/wiring.md) |
