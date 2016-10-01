#ifndef PERCENT_H
#define PERCENT_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Percent: public LightMode {
public:
  Percent(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Percent();
  void update(char* data);
  void tick();
  char* description();
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

  const char* name = "Percent";
};

#endif
