#ifndef SLIDE_H
#define SLIDE_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Slide: public LightMode {
public:
  Slide(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Slide();
  void update(char* data);
  void tick();
  char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  RgbColor color = RgbColor(255,255,255);
  int length = 2;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data);
  const char* name = "Slide";
};

#endif
