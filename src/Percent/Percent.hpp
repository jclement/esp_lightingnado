#ifndef PERCENT_H
#define PERCENT_H

/* =========== JSON data format ====================================
{
  "delay": 10,          // delay per update in (ms)
  "color": [255,0,0],   // target color for lights
  "percent": 10         // Percent of light strip to illuminate
}
*/

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Percent: public LightMode {
public:
  Percent(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Percent();
  void update(char* data);
  void tick();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;

  float percent = 0;
  float startingPercent = 0;
  float targetPercent = 0;

  RgbColor color = RgbColor(255,0,0);
  RgbColor startingColor = RgbColor(255,0,0);
  RgbColor targetColor = RgbColor(255,0,0);

  float progress = 1;

  int delayDuration = 10;
  void processData(bool init, char* data);
};

#endif
