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
  if (elapsed == 0) { return; } // don't bother updating the frame if no time has passed
  timeSinceLastRun += elapsed;
  // advance state machine until we've caught up
  while (timeSinceLastRun >= stateTime[state]) {
    timeSinceLastRun -= stateTime[state];
    if (state != OFF_STATE) { state = OFF_STATE; }
    else { state = (random(NUM_STATES - 1)) + 1; } // otherwise pick one at random that is NOT the off state
  }
  updateFrame();
}

const char* Halloween::description() {
  return "Halloween";
}

void Halloween::processData(char* data, bool reset) {
  StaticJsonBuffer<500> buf;
  if (reset) { 
    state = BEAT_STATE; 
  }
  updateFrame();
}

void Halloween::blinkFullStrip(int numLoops, int arrayLength, byte* dataArray, RgbColor colour) {
  int blockTime = stateTime[state] / (8 * numLoops * arrayLength);
  int loopTime = stateTime[state] / numLoops;
  int upperBlock = (timeSinceLastRun % loopTime) / blockTime;
  int lowerBlock = upperBlock == 0 ? (arrayLength - 1) : upperBlock - 1;
  strip->ClearTo(RgbColor::LinearBlend((dataArray[lowerBlock / 8] & (B10000000 >> (lowerBlock % 8))) ? colour : black, 
                                        (dataArray[upperBlock / 8] & (B10000000 >> (upperBlock % 8))) ? colour : black, 
                                        (float) ((timeSinceLastRun % loopTime) % blockTime) / ((float) blockTime)));  
}

void Halloween::updateFrame() {
  int stripLength = strip->PixelCount();
  strip->ClearTo(RgbColor(0,0,0));
  switch (state) {
    case OFF_STATE:
      break;
    case BEAT_STATE:
      blinkFullStrip(beatLoops, beatArrayLength, beatArray, beatColour);
      break;
    case FLASH_STATE:
      blinkFullStrip(flashLoops, flashArrayLength, flashArray, flashColour);
      break;
    case SLIDERS_STATE:
      strip->SetPixelColor(state, RgbColor(255, 0, 0));
      break;
    case TWINKLE_STATE:
      strip->SetPixelColor(state, RgbColor(0, 255, 0));
      break;
    case TRACERS_STATE:
      strip->SetPixelColor(state, RgbColor(0,0,255));
      break;
  }
  strip->Show();
}


//void Halloween::updateFrame() {
//  int stripLength = strip->PixelCount();
//  RgbColor currentColour;
//  strip->ClearTo(RgbColor(0,0,0));
//  int upperBlock = 0;
//  int lowerBlock = 0;
//  float blend = 0.0f;
//  RgbColor color1;
//  RgbColor color2;
//  int blockTime = stateTime[state] / (8 * (state == BEAT_STATE ? beatLoops : flashLoops) * (state == BEAT_STATE ? beatArrayLength : flashArrayLength));
//  byte* useArray = (state == BEAT_STATE ? beatArray : flashArray);
//  RgbColor useColour = (state == BEAT_STATE ? beatColour : flashColour);
//  int arrayMaxValue = (state == BEAT_STATE ? beatArrayLength : flashArrayLength) - 1;
//  int loopTime = stateTime[state] / (state == BEAT_STATE ? beatLoops : flashLoops);
//  switch (state) {
//    case OFF_STATE:
//      break;
//    case BEAT_STATE:
//    case FLASH_STATE:
//      upperBlock = (timeSinceLastRun % loopTime) / blockTime;
//      lowerBlock = upperBlock - 1;
//      if (lowerBlock < 0) { lowerBlock = arrayMaxValue; }
//      blend = (float) ((timeSinceLastRun % loopTime) % blockTime) / ((float) blockTime);
//      color1 = (useArray[lowerBlock / 8] & (B10000000 >> (lowerBlock % 8))) ? useColour : black;
//      color2 = (useArray[upperBlock / 8] & (B10000000 >> (upperBlock % 8))) ? useColour : black;
//      currentColour = RgbColor::LinearBlend(color1, color2, blend);
//      strip->ClearTo(currentColour);
//      break;
//    case SLIDERS_STATE: // orange sliders
//      strip->SetPixelColor(state, RgbColor(255, 0, 0));
//      break;
//    case TWINKLE_STATE: // orange twinkle
//      strip->SetPixelColor(state, RgbColor(0, 255, 0));
//      break;
//    case TRACERS_STATE:
//      strip->SetPixelColor(state, RgbColor(0,0,255));
//      break;
//  }
//  strip->Show();
//}

Halloween::~Halloween() {

}
