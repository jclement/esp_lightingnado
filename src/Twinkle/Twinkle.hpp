#ifndef TWINKLE_H
#define TWINKLE_H

/* =========== JSON data format ====================================
{
  "delay": 10,          // delay per update in (ms)
  "decay": 5,           // rate at which twinkles loose intensite per update
  "spawn": 10           // rate at which new twinkles are spawned.  Once, every N updates. ish
}
*/

#include <NeoPixelBus.h>
#include "LightMode.hpp"

class Twinkle: public LightMode {
public:
  Twinkle(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Twinkle();
  void update(char* data);
  void tick();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  int delayDuration = 10;
  int spawnRate = 10;
  int decayRate = 5;
  void processData(char* data);
};

#endif
