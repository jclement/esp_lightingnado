#include "Halloween.hpp"
#include "ArduinoJson.h"

Halloween::Halloween(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  processData(data, true);
}

void Halloween::update(char* data) {
  processData(data, false);
}

void Halloween::tick(unsigned long elapsed) {
  timeSinceLastRun += elapsed;
  // advance state machine until we've caught up
  while (timeSinceLastRun >= stateTime[state]) {
    timeSinceLastRun -= stateTime[state];
    /*if (state != OFF_STATE) { state = OFF_STATE; }
    else { state = random(NUM_STATES); } // otherwise pick one at random*/
  }
  updateFrame();
}

const char* Halloween::description() {
  return "Halloween";
}

void Halloween::processData(char* data, bool reset) {
  StaticJsonBuffer<500> buf;
  if (reset) { 
    state = 1; 
  }
  updateFrame();
}

void Halloween::updateFrame() {
  int stripLength = strip->PixelCount();
  int fadePosition;
  RgbColor currentColour;
  strip->ClearTo(RgbColor(0,0,0));
  int upperBlock = 0;
  int lowerBlock = 0;
  float blend = 0.0f;
  RgbColor color1;
  RgbColor color2;
  switch (state) {
    case OFF_STATE:
      break;
    case BEAT_STATE:
      // set fadePosition based on our proportional position between bits in the current block
      // 8 bits to the pattern, 5000 msec per cycle
      //[625, 1250, 1875, 2500, 3125, 3750, 4375, 5000]
      upperBlock = (timeSinceLastRun % 5000) / 625;
      lowerBlock = upperBlock - 1;
      if (lowerBlock < 0) { lowerBlock = 7; }
      blend = (float) ((timeSinceLastRun % 5000) % 625) / 625.0f;
      color1 = ((beatArray[0] << lowerBlock) & B10000000) ? beatColour : black;
      color2 = ((beatArray[0] << upperBlock) & B10000000) ? beatColour : black;
      currentColour = RgbColor::LinearBlend(color1, color2, blend);
      strip->ClearTo(currentColour);
      break;
    case SLIDERS_STATE: // orange sliders
      strip->SetPixelColor(state, RgbColor(220, 78, 0));
      break;
    case TWINKLE_STATE: // orange twinkle
      strip->SetPixelColor(state, RgbColor(220, 78, 0));
      break;
    case TRACERS_STATE:
      strip->SetPixelColor(state, RgbColor(0,255,0));
    case FLASH_STATE: // flash white
      if ((timeSinceLastRun / 10) % 10 == 0) {
        strip->ClearTo(RgbColor(255,255,255));
      }
      break;      
  }
  strip->Show();
}

Halloween::~Halloween() {

}
