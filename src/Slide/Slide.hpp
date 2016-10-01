#ifndef SLIDE_H
#define SLIDE_H

/* =========== JSON data format ====================================
{
  "delay": 100,          // delay per update in (ms)
  "length": 5,           // length of moving segement
  "right": false,        // direction of travel
  "color":[200,100,0]    // colors R,G,B
}
*/

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Slide: public LightMode {
public:
  Slide(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Slide();
  void update(char* data);
  void tick();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  RgbColor color = RgbColor(255,255,255);
  int length = 2;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data);
};

#endif
