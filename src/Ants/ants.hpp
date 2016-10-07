#ifndef ANTS_H
#define ANTS_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Ants: public LightMode {
public:
  Ants(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Ants();
  void update(char* data);
  void tick();
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data);
  unsigned long lastRun = 0;
};

#endif
