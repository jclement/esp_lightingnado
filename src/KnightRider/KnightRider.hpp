#ifndef KNIGHT_RIDER_H
#define KNIGHT_RIDER_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class KnightRider: public LightMode {
public:
  KnightRider(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~KnightRider();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  RgbColor color = RgbColor(255,0,0);
  int state = 0; // 0 is left on solid, 1 is moving right, 2 is right on solid, 3 is moving left
  int endHoldTime = 250;
  int slideTime = 1500;
  int width = 20;
  int length = 0;
  void processData(char* data, bool reset);
  void updateFrame();
  unsigned int delayDuration = 0;
  unsigned long timeSinceLastRun = 0;
};

#endif
