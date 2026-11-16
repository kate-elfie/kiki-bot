#pragma once

#include "display.h"

enum class Mood { Neutral, Happy, Sad, Angry, Surprised, Sleepy, Love };
constexpr int kMoodCount = 7;

const char* moodName(Mood m);
bool moodFromName(const String& name, Mood& out);

// Two animated eyes: smooth mood changes, random blinks, looking around.
class Eyes {
 public:
  void begin(LGFX& tft);
  void setMood(Mood m);
  Mood mood() const { return _mood; }
  void update();  // call every loop()

 private:
  struct Shape {
    float w, h;          // eye size in px
    float lidTop;        // 0..1 of height covered flat from the top (sleepy)
    float lidInner;      // 0..1 slanted lid on the inner corner (angry)
    float lidOuter;      // 0..1 slanted lid on the outer corner (sad)
    float lidBottom;     // 0..1 curved cheek from below (happy)
  };

  static Shape shapeFor(Mood m);
  void tickBlink(uint32_t now);
  void tickLook(uint32_t now);
  void draw();
  void drawEye(float cx, float cy, bool left);
  void drawHeart(float cx, float cy, float size);

  LGFX* _tft = nullptr;
  LGFX_Sprite _frame;

  Mood _mood = Mood::Neutral;
  Shape _cur{}, _target{};
  float _lookX = 0, _lookY = 0, _lookTargetX = 0, _lookTargetY = 0;
  float _blink = 1;  // 1 = open, ~0 = closed
  bool _blinking = false;
  uint32_t _blinkStart = 0, _nextBlink = 0, _nextLook = 0, _lastFrame = 0;
};
