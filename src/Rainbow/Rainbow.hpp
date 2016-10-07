#ifndef RAINBOW_H
#define RAINBOW_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Rainbow: public LightMode {
public:
  Rainbow(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Rainbow();
  void update(char* data);
  void tick();
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  bool directionRight = true;
  int delayDuration = 10;
  float brightness = 1;
  int step = 1;
  void processData(char* data);
  unsigned long lastRun = 0;
};

#endif
