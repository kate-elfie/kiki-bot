// Kiki step 1: emotion eyes on the round display, touch to cheer her up.
// Serial Monitor (115200, COM port): type a mood name, "demo", or "touch".

#include <Arduino.h>

#include "display.h"
#include "eyes.h"
#include "pins.h"

constexpr uint32_t kLoveHoldMs = 1500;   // hold the touch this long → love
constexpr uint32_t kTouchMoodMs = 4000;  // touch mood lasts this long after release
constexpr uint32_t kDemoStepMs = 3000;

// Touch pad = foil on a wire, read by the S3's built-in touch sensor.
// The reading goes UP when touched. Pressed above +30% of the idle baseline,
// released below +15%: the gap stops flicker at the edge.
constexpr float kTouchOn = 1.30f;
constexpr float kTouchOff = 1.15f;
constexpr int kTouchCalibrationReads = 50;
constexpr uint32_t kTouchPrintMs = 200;

LGFX tft;
Eyes eyes;

bool demo = true;  // cycle through every mood until a mood is typed
uint32_t nextDemoStep = 0;

bool touched = false;
bool reacting = false;  // showing a touch mood, will return to moodBeforeTouch
uint32_t touchStart = 0, touchEnd = 0;
Mood moodBeforeTouch = Mood::Neutral;

float touchBaseline = 0;  // idle reading, measured at boot
bool printTouch = false;  // "touch" command: print readings to tune kTouchOn
uint32_t nextTouchPrint = 0;

void printHelp() {
  Serial.println("Kiki commands: neutral happy sad angry surprised sleepy love demo touch");
}

void calibrateTouch() {
  uint64_t sum = 0;
  for (int i = 0; i < kTouchCalibrationReads; i++) {
    sum += touchRead(PIN_TOUCH);
    delay(5);
  }
  touchBaseline = float(sum) / kTouchCalibrationReads;
  Serial.printf("touch baseline: %.0f (keep hands off the pad while Kiki boots)\n", touchBaseline);
}

bool readTouch(uint32_t now) {
  uint32_t value = touchRead(PIN_TOUCH);
  bool down = value > touchBaseline * (touched ? kTouchOff : kTouchOn);
  if (!down) touchBaseline += (value - touchBaseline) * 0.01f;  // follow slow drift (humidity, heat)

  if (printTouch && now >= nextTouchPrint) {
    Serial.printf("touch: %u  baseline: %.0f  touched above: %.0f\n", value, touchBaseline,
                  touchBaseline * kTouchOn);
    nextTouchPrint = now + kTouchPrintMs;
  }
  return down;
}

void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.isEmpty()) return;

  if (cmd == "demo") {
    demo = !demo;
    Serial.printf("demo %s\n", demo ? "on" : "off");
    return;
  }
  if (cmd == "touch") {
    printTouch = !printTouch;
    Serial.printf("touch readings %s\n", printTouch ? "on" : "off");
    return;
  }
  Mood m;
  if (moodFromName(cmd, m)) {
    demo = false;
    reacting = false;  // a typed mood wins over an earlier touch
    eyes.setMood(m);
    Serial.printf("mood: %s\n", moodName(m));
  } else {
    Serial.printf("unknown command: \"%s\"\n", cmd.c_str());
    printHelp();
  }
}

void handleTouch(uint32_t now) {
  bool down = readTouch(now);

  if (down && !touched) {
    touched = true;
    touchStart = now;
    if (!reacting) moodBeforeTouch = eyes.mood();
    reacting = true;
    eyes.setMood(Mood::Happy);
  } else if (down && now - touchStart >= kLoveHoldMs && eyes.mood() != Mood::Love) {
    eyes.setMood(Mood::Love);
  } else if (!down && touched) {
    touched = false;
    touchEnd = now;
  } else if (!down && reacting && now - touchEnd >= kTouchMoodMs) {
    reacting = false;
    eyes.setMood(moodBeforeTouch);
  }
}

void handleDemo(uint32_t now) {
  if (!demo || reacting || now < nextDemoStep) return;
  Mood next = Mood((int(eyes.mood()) + 1) % kMoodCount);
  eyes.setMood(next);
  Serial.printf("mood: %s\n", moodName(next));
  nextDemoStep = now + kDemoStepMs;
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  randomSeed(esp_random());
  calibrateTouch();

  tft.init();
  eyes.begin(tft);
  printHelp();
}

void loop() {
  uint32_t now = millis();
  handleSerial();
  handleTouch(now);
  handleDemo(now);
  eyes.update();
}
