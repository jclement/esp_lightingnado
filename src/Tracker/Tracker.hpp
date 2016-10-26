#ifndef TRACKER_H
#define TRACKER_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

#define DATA_ARR_LENGTH 100

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
  void updateFrame();
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  RgbColor colour = RgbColor(255,255,255);
  RgbColor black = RgbColor(0,0,0);
  int dataArray[DATA_ARR_LENGTH];
  int stripLength = 0;
  int fadeTime = 200; // in milliseconds
  int fadeDelay = 100;
  RgbColor calculateColour(int millis);
  RgbColor stripIsWider(int pixelIndex);
  RgbColor stripIsNarrower(int pixelIndex);
};

#endif
