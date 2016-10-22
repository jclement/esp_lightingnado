#include "KnightRider.hpp"
#include "ArduinoJson.h"

KnightRider::KnightRider(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  stripLength = strip->PixelCount();
  processData(data, true);
}

void KnightRider::update(char* data) {
  this->processData(data, false);
}

void KnightRider::tick(unsigned long elapsed) {  
  timeSinceLastRun += elapsed;
  // advance state machine until we've caught up
  while (timeSinceLastRun >= stateTime[state]) {
    timeSinceLastRun -= stateTime[state++];
    if (!fixedColour) {
      colour = RgbColor(HsbColor(((float) random(1000) / 1000.0f), 1.0f, 1.0f));
    }
    if (state > 3) { 
      state = 0; 
    }
  }
  updateFrame();
}

const char* KnightRider::description() {
  return "Knight Rider";
}

void KnightRider::processData(char* data, bool reset) {
  StaticJsonBuffer<500> buf;
  JsonObject& root = buf.parseObject(data);
  if (reset) { 
    state = 0; 
  }
  if (root.success()) {
    if (root.containsKey("endDelay")) {
      stateTime[1] = root["endDelay"];

      if (stateTime[1] < 0) {
        stateTime[1] = 0;
      } else if (stateTime[1] > 10000) {
        stateTime[1] = 10000;
      }
      stateTime[3] = stateTime[1];
    }
    if (root.containsKey("sweepTime")) {
      stateTime[0] = root["sweepTime"];
      if (stateTime[0] < 0) {
        stateTime[0] = 0;
      } else if (stateTime[0] > 10000) {
        stateTime[0] = 10000;
      }
      stateTime[2] = stateTime[0];
    }
    if (root.containsKey("colour")) {
      fixedColour = true;
      colour = RgbColor(root["color"][0], root["color"][1], root["color"][2]);
    } else {
      fixedColour = false;
    }
  }
  updateFrame();
}

void KnightRider::updateFrame() {
  int offset;
  int sliderWidth = (width * stripLength) / 100;
  int sliderPos;
  int currentPos;
  int i;
  if (state == 1 || state == 3) {
    // sliderPos starts at negative half a width and ends at positive half a width
    sliderPos = (sliderWidth / (-2)) + ((timeSinceLastRun * (100 + width) * stripLength) / (stateTime[state] * 100));
  }
  strip->ClearTo(RgbColor(0,0,0));
  switch (state) {
    case 0:
      strip->SetPixelColor(0, colour);
      break;
    case 1:      
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i >= (sliderPos - sliderWidth/2) && i < (sliderPos + sliderWidth/2)) {
          strip->SetPixelColor(i, RgbColor::LinearBlend(colour,RgbColor(0,0,0),(float) ((sliderPos + sliderWidth/2) - i) / (float) (sliderWidth)));
        } else {
          strip->SetPixelColor(i, colour); // unused
        }
      }
      if ((sliderPos + sliderWidth/2) >= stripLength) {
        strip->SetPixelColor(stripLength - 1, colour);
      }
      break;
    case 2:      
      strip->SetPixelColor(stripLength - 1, colour);
      break;
    case 3:
      sliderPos = stripLength - sliderPos;
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i <= (sliderPos + sliderWidth/2) && i > (sliderPos - sliderWidth/2)) {
          strip->SetPixelColor(i, RgbColor::LinearBlend(colour,RgbColor(0,0,0),(float) (i - (sliderPos - sliderWidth/2)) / (float) (sliderWidth)));
        } else {
          strip->SetPixelColor(i, colour);
        }
      }
      if ((sliderPos - sliderWidth/2) <= 0) {
        strip->SetPixelColor(0, colour);
      }
      break;      
  }
  strip->Show();
}

KnightRider::~KnightRider() {

}
