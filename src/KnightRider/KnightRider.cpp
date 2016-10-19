#include "KnightRider.hpp"
#include "ArduinoJson.h"

KnightRider::KnightRider(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data, true);
}

void KnightRider::update(char* data) {
  this->processData(data, false);
}

void KnightRider::tick(unsigned long elapsed) {  
  timeSinceLastRun += elapsed;
  // advance state machine until we've caught up
  while (timeSinceLastRun >= this->stateTime[this->state]) {
    timeSinceLastRun -= this->stateTime[this->state++];
    color = RgbColor(HsbColor(((float) random(1000) / 1000.0f), 1.0f, 1.0f));
    if (this->state > 3) { 
      this->state = 0; 
    }
  }
  updateFrame();
}

const char* KnightRider::description() {
  return "Knight Rider";
}

void KnightRider::processData(char* data, bool reset) {
  StaticJsonBuffer<500> buf;
  if (reset) { 
    this->state = 0; 
  }
  // one day we'll process some JSON here... one day
  updateFrame();
}

void KnightRider::updateFrame() {
  int stripLength = this->strip->PixelCount();
  int offset;
  int sliderWidth = (this->width * stripLength) / 100;
  int sliderPos;
  int currentPos;
  int i;
  if (this->state == 1 || this->state == 3) {
    // sliderPos starts at negative half a width and ends at positive half a width
    sliderPos = (sliderWidth / (-2)) + ((timeSinceLastRun * (100 + this->width) * stripLength) / (this->stateTime[this->state] * 100));
  }
  this->strip->ClearTo(RgbColor(0,0,0));
  switch (this->state) {
    case 0:
      this->strip->SetPixelColor(0, this->color);
      break;
    case 1:      
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i >= (sliderPos - sliderWidth/2) && i < (sliderPos + sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor::LinearBlend(this->color,RgbColor(0,0,0),(float) ((sliderPos + sliderWidth/2) - i) / (float) (sliderWidth)));
        } else {
          this->strip->SetPixelColor(i, this->color); // unused
        }
      }
      if ((sliderPos + sliderWidth/2) >= stripLength) {
        this->strip->SetPixelColor(stripLength - 1, this->color);
      }
      break;
    case 2:      
      this->strip->SetPixelColor(stripLength - 1, this->color);
      break;
    case 3:
      sliderPos = stripLength - sliderPos;
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i <= (sliderPos + sliderWidth/2) && i > (sliderPos - sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor::LinearBlend(this->color,RgbColor(0,0,0),(float) (i - (sliderPos - sliderWidth/2)) / (float) (sliderWidth)));
        } else {
          this->strip->SetPixelColor(i, this->color);
        }
      }
      if ((sliderPos - sliderWidth/2) <= 0) {
        this->strip->SetPixelColor(0, this->color);
      }
      break;      
  }
  this->strip->Show();
}

KnightRider::~KnightRider() {

}
