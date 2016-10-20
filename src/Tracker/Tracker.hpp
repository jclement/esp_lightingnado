#ifndef TRACKER_H
#define TRACKER_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Tracker: public LightMode {
public:
  Tracker(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Tracker();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description() { return "Tracker"; };
  const char* detailsTopic() { return "tracker"; };
  void process(char* data);
private:
  void processData(char* data);
  void newPixels(char* data);
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  int decayRate = 10;
  RgbColor color = RgbColor(200,200,200);
  unsigned long timeSinceLastRun = 0;
};

#endif
