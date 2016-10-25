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
  int stateTime[NUM_STATES] = {5000,30000,10000,10000,10000,2000};
  int length = 0;
  unsigned int currentDelay = 0;
  bool directionRight = true;
  int delayDuration = 10;
  void processData(char* data, bool reset);
  void updateFrame();
  void blinkFullStrip(int numLoops, int arrayLength, byte* dataArray, RgbColor colour);
  unsigned long timeSinceLastRun = 0;
  const int flashArrayLength = 16;
  const int beatArrayLength = 4;
  const int flashLoops = 2;
  const int beatLoops = 10;
  byte beatArray[4] = {B00000000, B00010001, B00000000, B00000000};  
  byte flashArray[16] = {B00000001, B00000001, B00010001, B00000001, B00000001, B00000001, B00001111, B00001111,
                        B00010001, B00010001, B01010101, B01010101, B00000011, B00001111, B11111111, B11111111};
  RgbColor flashColour = RgbColor(40,40,40);
  RgbColor beatColour = RgbColor(80,0,0);
  RgbColor black = RgbColor(0,0,0);
  byte sliderPositions[2];
  byte tracerPositions[4];
  byte numTracers = 0;
};

#endif
