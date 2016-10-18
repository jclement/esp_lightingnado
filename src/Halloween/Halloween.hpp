#ifndef HALLOWEEN_H
#define HALLOWEEN_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Halloween: public LightMode {
public:
  Halloween(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Halloween();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  RgbColor color = RgbColor(255,255,255);
  int state = 0;
  int length = 0;
  unsigned int currentDelay = 0;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data, bool reset);
  void updateFrame();
  unsigned long timeSinceLastRun = 0;
};

#endif
