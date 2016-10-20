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
    if (state > 0) { state = 0; } // if we aren't in the off state return to it
    else { state = random(5); } // otherwise pick one at random
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
  switch (state) {
    case 0: // easy case!  We want it black!
      break;
    case 1: // slow orange fade
      fadePosition = ((timeSinceLastRun % 5000) - 2500);
      if  (fadePosition <= 0) {
        fadePosition = 2500 + fadePosition;
      } else {
        fadePosition = 2500 - fadePosition;
      }
      currentColour = RgbColor::LinearBlend(RgbColor(0,0,0),RgbColor(220,78,0),(float) fadePosition / 2500.0f);
      strip->ClearTo(currentColour);
      break;
    case 2: // orange sliders
      strip->SetPixelColor(state, RgbColor(220, 78, 0));
      break;
    case 3: // orange twinkle
      strip->SetPixelColor(state, RgbColor(220, 78, 0));
      break;
    case 4: // flash white
      if ((timeSinceLastRun / 10) % 10 == 0) {
        strip->ClearTo(RgbColor(255,255,255));
      }
      break;      
  }
  strip->Show();
}

Halloween::~Halloween() {

}
