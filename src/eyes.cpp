#include "eyes.h"

namespace {

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

constexpr int kSize = 240;
constexpr float kCenter = kSize / 2.0f;
constexpr float kEyeGap = 100;  // distance between the two eye centres
constexpr uint16_t kBg = rgb565(0, 0, 0);
constexpr uint16_t kEyeColor = rgb565(0, 220, 255);
constexpr uint16_t kLoveColor = rgb565(255, 80, 140);

constexpr uint32_t kFrameMs = 25;  // ~40 fps
constexpr uint32_t kBlinkMs = 160;
constexpr float kEase = 0.2f;      // share of the remaining distance moved per frame

float approach(float cur, float target) { return cur + (target - cur) * kEase; }

struct NamedMood {
  const char* name;
  Mood mood;
};

constexpr NamedMood kMoods[kMoodCount] = {
    {"neutral", Mood::Neutral},     {"happy", Mood::Happy},   {"sad", Mood::Sad},
    {"angry", Mood::Angry},         {"surprised", Mood::Surprised},
    {"sleepy", Mood::Sleepy},       {"love", Mood::Love},
};

}  // namespace

const char* moodName(Mood m) {
  for (const auto& entry : kMoods) {
    if (entry.mood == m) return entry.name;
  }
  return "?";
}

bool moodFromName(const String& name, Mood& out) {
  for (const auto& entry : kMoods) {
    if (name == entry.name) {
      out = entry.mood;
      return true;
    }
  }
  return false;
}

void Eyes::begin(LGFX& tft) {
  _tft = &tft;
  _frame.setColorDepth(16);
  if (!_frame.createSprite(kSize, kSize)) {
    // Internal RAM full: fall back to the 8MB PSRAM (slower but fine)
    _frame.setPsram(true);
    _frame.createSprite(kSize, kSize);
  }
  _cur = _target = shapeFor(_mood);
  uint32_t now = millis();
  _nextBlink = now + 1500;
  _nextLook = now + 2000;
}

void Eyes::setMood(Mood m) {
  _mood = m;
  _target = shapeFor(m);
}

Eyes::Shape Eyes::shapeFor(Mood m) {
  //                       w    h    top    inner  outer  bottom
  switch (m) {
    case Mood::Happy:     return {74, 80, 0, 0, 0, 0.45f};
    case Mood::Sad:       return {70, 70, 0.1f, 0, 0.5f, 0};
    case Mood::Angry:     return {70, 70, 0, 0.55f, 0, 0};
    case Mood::Surprised: return {84, 100, 0, 0, 0, 0};
    case Mood::Sleepy:    return {72, 80, 0.6f, 0, 0, 0};
    case Mood::Love:      return {70, 70, 0, 0, 0, 0};
    case Mood::Neutral:
    default:              return {70, 80, 0, 0, 0, 0};
  }
}

void Eyes::update() {
  uint32_t now = millis();
  if (now - _lastFrame < kFrameMs) return;
  _lastFrame = now;

  tickBlink(now);
  tickLook(now);
  _cur.w = approach(_cur.w, _target.w);
  _cur.h = approach(_cur.h, _target.h);
  _cur.lidTop = approach(_cur.lidTop, _target.lidTop);
  _cur.lidInner = approach(_cur.lidInner, _target.lidInner);
  _cur.lidOuter = approach(_cur.lidOuter, _target.lidOuter);
  _cur.lidBottom = approach(_cur.lidBottom, _target.lidBottom);
  draw();
}

void Eyes::tickBlink(uint32_t now) {
  if (!_blinking && now >= _nextBlink) {
    _blinking = true;
    _blinkStart = now;
  }
  if (!_blinking) return;

  float t = float(now - _blinkStart) / kBlinkMs;  // 0..1 through the blink
  if (t >= 1) {
    _blinking = false;
    _blink = 1;
    _nextBlink = now + (_mood == Mood::Sleepy ? random(1500, 3000) : random(2000, 6000));
    return;
  }
  _blink = max(0.08f, fabsf(1 - 2 * t));  // open → closed → open
}

void Eyes::tickLook(uint32_t now) {
  if (now >= _nextLook) {
    if (random(100) < 35) {
      _lookTargetX = 0;
      _lookTargetY = 0;
    } else {
      _lookTargetX = random(-25, 26);
      _lookTargetY = random(-15, 16);
    }
    _nextLook = now + random(1200, 4000);
  }
  _lookX = approach(_lookX, _lookTargetX);
  _lookY = approach(_lookY, _lookTargetY);
}

void Eyes::draw() {
  _frame.fillScreen(kBg);
  float cy = kCenter + _lookY;
  drawEye(kCenter - kEyeGap / 2 + _lookX, cy, true);
  drawEye(kCenter + kEyeGap / 2 + _lookX, cy, false);
  _frame.pushSprite(_tft, 0, 0);
}

void Eyes::drawEye(float cx, float cy, bool left) {
  if (_mood == Mood::Love) {
    float pulse = 1 + 0.08f * sinf(millis() / 150.0f);
    drawHeart(cx, cy, _cur.w * pulse);
    return;
  }

  float w = _cur.w, h = _cur.h * _blink;
  float x = cx - w / 2, top = cy - h / 2;
  _frame.fillRoundRect(x, top, w, h, min(w, h) * 0.3f, kEyeColor);

  // Lids: background-coloured shapes drawn over the eye
  constexpr float pad = 2;
  float innerX = left ? x + w + pad : x - pad;  // the corner nearer the nose
  float outerX = left ? x - pad : x + w + pad;
  if (_cur.lidTop > 0.01f) {
    _frame.fillRect(x - pad, top - pad, w + 2 * pad, h * _cur.lidTop + pad, kBg);
  }
  if (_cur.lidInner > 0.01f) {
    _frame.fillTriangle(innerX, top - pad, outerX, top - pad, innerX, top + h * _cur.lidInner, kBg);
  }
  if (_cur.lidOuter > 0.01f) {
    _frame.fillTriangle(outerX, top - pad, innerX, top - pad, outerX, top + h * _cur.lidOuter, kBg);
  }
  if (_cur.lidBottom > 0.01f) {
    float ry = h * 0.6f;
    _frame.fillEllipse(cx, top + h + ry - h * _cur.lidBottom, w * 0.8f, ry, kBg);
  }
}

void Eyes::drawHeart(float cx, float cy, float size) {
  float r = size * 0.28f;
  float dx = size * 0.24f;
  float lobeY = cy - size * 0.12f;
  _frame.fillCircle(cx - dx, lobeY, r, kLoveColor);
  _frame.fillCircle(cx + dx, lobeY, r, kLoveColor);
  _frame.fillTriangle(cx - dx - r * 0.97f, lobeY + r * 0.25f, cx + dx + r * 0.97f,
                      lobeY + r * 0.25f, cx, cy + size * 0.5f, kLoveColor);
}
