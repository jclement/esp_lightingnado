#ifndef HALLOWEEN_H
#define HALLOWEEN_H

#include <NeoPixelBus.h>
#include "LightMode.hpp"

#define OFF_STATE 0
#define BEAT_STATE 1
#define SLIDERS_STATE 2
#define TWINKLE_STATE 3
#define TRACERS_STATE 4
#define FLASH_STATE 5
#define NUM_STATES 6

class Halloween: public LightMode {
public:
  Halloween(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data);
  ~Halloween();
  void update(char* data);
  void tick(unsigned long elapsed);
  const char* description();
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  int state = 0;
  int stateTime[NUM_STATES] = {5000,30000,10000,10000,10000,1500};
  int length = 0;
  unsigned int currentDelay = 0;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data, bool reset);
  void updateFrame();
  unsigned long timeSinceLastRun = 0;
  byte flashArray[4] = {B10001000, B10001000, B10101010, B00001111};
  byte beatArray[1] = {B00101000};  
  RgbColor beatColour = RgbColor(80,0,0);
  RgbColor black = RgbColor(0,0,0);
  byte sliderPositions[2];
  byte tracerPositions[4];
  byte numTracers = 0;
};

#endif
