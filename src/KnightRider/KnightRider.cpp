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
  // advance state machine
  while (((this->state == 0 || this->state == 2) && (timeSinceLastRun >= this->endHoldTime)) ||
         ((this->state == 1 || this->state == 3) && (timeSinceLastRun >= this->slideTime))) {
    if ((this->state == 0) || (this->state == 2)) {
      timeSinceLastRun -= this->endHoldTime;
    }
    else if ((this->state == 1) || (this->state == 3)) {
      timeSinceLastRun -= this->slideTime;
    }
    this->state++;
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
    this->strip->ClearTo(RgbColor(0,0,0));
  }
  updateFrame();
}

void KnightRider::updateFrame() {
  int stripLength = this->strip->PixelCount();
  int offset = 0;
  int width = (this->width * stripLength) / 100;
  int start;
  int currentPos;
  if (this->state == 1 || this->state == 3) {
    offset = (timeSinceLastRun * stripLength) / (this->slideTime);
  }
  this->strip->ClearTo(RgbColor(0,0,0));
  switch (this->state) {
    case 0:
      this->strip->SetPixelColor(0, this->color);
      break;
    case 1:
      start = offset - (width / 2);
      currentPos = start;
      while (currentPos < (start + width) && currentPos < (stripLength - 1)) {
        this->strip->SetPixelColor(currentPos, this->color);
        currentPos++;
      }
      break;
    case 2:      
      this->strip->SetPixelColor(stripLength - 1, this->color);
      break;
    case 3:
      start = (stripLength - offset) + (width / 2);
      currentPos = start;
      while (currentPos >= (start - width) && currentPos >= 0) {
        this->strip->SetPixelColor(currentPos, this->color);
        currentPos--;
      }
      break;      
  }
  this->strip->Show();
}

KnightRider::~KnightRider() {

}
