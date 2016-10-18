#include "Halloween.hpp"
#include "ArduinoJson.h"

Halloween::Halloween(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data, true);
}

void Halloween::update(char* data) {
  this->processData(data, false);
}

void Halloween::tick(unsigned long elapsed) {
  timeSinceLastRun += elapsed;
  if (timeSinceLastRun < this->delayDuration) return;
  while (timeSinceLastRun >= this->delayDuration) {
    timeSinceLastRun -= this->delayDuration;
    //advance frame counter
  }
  this->timeSinceLastRun = 0;

  updateFrame();
}

const char* Halloween::description() {
  return "Halloween";
}

void Halloween::processData(char* data, bool reset) {
  StaticJsonBuffer<500> buf;
  JsonObject& root = buf.parseObject(data);
  if (reset) { 
    state = 0; 
    this->strip->ClearTo(RgbColor(0,0,0));
  }

  if (root.containsKey("length")) {
    this->length = root["length"];
  }

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("right")) {
    this->directionRight = root["right"];
  }

  if (root.containsKey("color")) {
    this->color = RgbColor(root["color"][0], root["color"][1], root["color"][2]);
  }

  if (this->length < 1 || this->length > this->strip->PixelCount()) {
    this->length = 1;
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }
  updateFrame();
}

void Halloween::updateFrame() {
  for(int i=0; i<this->length; i++) {
    this->strip->SetPixelColor(i, this->color);
  }  
  this->strip->Show();
}

Halloween::~Halloween() {

}
