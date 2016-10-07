#ifndef TRACKER_H
#define TRACKER_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Tracker: public LightMode {
public:
  Tracker(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Tracker();
  void update(char* data);
  void tick();
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  int decayRate = 1;
  void processData(char* data);
  const char* name = "Tracker";
  RgbColor color = RgbColor(200,200,200);
  unsigned long lastRun = 0;
};

#endif
