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
  int state = 0; // 0 is off, 1 is dim orange, 2 is orange sliders, 3 is orange twinkle, 4 is flashing white
  int stateTime[5] = {5000,30000,10000,10000,500};
  int length = 0;
  unsigned int currentDelay = 0;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data, bool reset);
  void updateFrame();
  unsigned long timeSinceLastRun = 0;
};

#endif
