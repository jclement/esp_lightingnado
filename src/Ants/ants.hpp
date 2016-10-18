#ifndef ANTS_H
#define ANTS_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Ants: public LightMode {
public:
  Ants(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Ants();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data);
  unsigned long accumulatedTime = 0;
  unsigned int numColours = 0;
  unsigned int lastStart = 0;
  RgbColor *colours;
};

#endif
