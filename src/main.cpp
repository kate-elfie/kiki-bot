// Kiki step 1: emotion eyes on the round display, touch to cheer her up.
// Serial Monitor (115200, COM port): type a mood name, or "demo".

#include <Arduino.h>

#include "display.h"
#include "eyes.h"
#include "pins.h"

constexpr uint32_t kLoveHoldMs = 1500;   // hold the touch this long → love
constexpr uint32_t kTouchMoodMs = 4000;  // touch mood lasts this long after release
constexpr uint32_t kDemoStepMs = 3000;

LGFX tft;
Eyes eyes;

bool demo = true;  // cycle through every mood until a mood is typed
uint32_t nextDemoStep = 0;

bool touched = false;
bool reacting = false;  // showing a touch mood, will return to moodBeforeTouch
uint32_t touchStart = 0, touchEnd = 0;
Mood moodBeforeTouch = Mood::Neutral;

void printHelp() {
  Serial.println("Kiki commands: neutral happy sad angry surprised sleepy love demo");
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
  bool down = digitalRead(PIN_TOUCH) == HIGH;

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
  pinMode(PIN_TOUCH, INPUT_PULLDOWN);  // reads LOW when the sensor is not plugged in
  randomSeed(esp_random());

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
