#ifndef TWINKLE_H
#define TWINKLE_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Twinkle: public LightMode {
public:
  Twinkle(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Twinkle();
  void update(char* data);
  void tick();
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  int delayDuration = 10;
  int spawnRate = 10;
  int decayRate = 5;
  void processData(char* data);
  const char* name = "Twinkle";
  unsigned long lastRun = 0;
};

#endif
