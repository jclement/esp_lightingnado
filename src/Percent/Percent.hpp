#ifndef PERCENT_H
#define PERCENT_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Percent: public LightMode {
public:
  Percent(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data, int start, int length);
  ~Percent();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;

  int start;
  int length;

  float percent = 0;
  float startingPercent = 0;
  float targetPercent = 0;

  RgbColor color = RgbColor(255,0,0);
  RgbColor startingColor = RgbColor(255,0,0);
  RgbColor targetColor = RgbColor(255,0,0);

  float progress = 1;

  int delayDuration = 10;
  void processData(bool init, char* data);

  char name[20];
  unsigned long timeSinceLastRun = 0;
};

#endif
