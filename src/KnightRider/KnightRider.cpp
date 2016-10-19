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
    if (this->state > 3) { this->state = 0; }
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
      sliderPos = offset - (width / 2);
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i >= (sliderPos - sliderWidth/2) && i < sliderPos) {
          this->strip->SetPixelColor(i, RgbColor::LinearBlend(RgbColor(0,0,0),this->color,(float) (sliderPos - i) / (float) (sliderWidth / 2)));            
        } else {
          this->strip->SetPixelColor(i, this->color);
        }
      }
      break;
    case 2:      
      this->strip->SetPixelColor(stripLength - 1, this->color);
      break;
    case 3:
      sliderPos = stripLength - (offset + (width / 2));
      for (i = 0;i < stripLength;i++) {
        if (i < (sliderPos - sliderWidth/2) || i > (sliderPos + sliderWidth/2)) {
          this->strip->SetPixelColor(i, RgbColor(0,0,0));
        } else if (i < (sliderPos + sliderWidth/2) && i >= sliderPos) {
          this->strip->SetPixelColor(i, RgbColor::LinearBlend(RgbColor(0,0,0),this->color,(float) (i - sliderPos) / (float) (sliderWidth / 2)));            
        } else {
          this->strip->SetPixelColor(i, this->color);
        }
      }
      break;      
  }
  this->strip->Show();
}

KnightRider::~KnightRider() {

}
