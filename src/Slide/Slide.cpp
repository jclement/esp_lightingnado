#include "Slide.hpp"
#include "ArduinoJson.h"

Slide::Slide(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Slide::update(char* data) {
  this->processData(data);
}

void Slide::tick() {
  if (abs(millis() - this->lastRun) < this->delayDuration) return;
  this->lastRun = millis();

  if (this->directionRight) {
    this->strip->RotateRight(this->step);
  } else {
    this->strip->RotateLeft(this->step);
  }

  this->strip->Show();
}

const char* Slide::description() {
  return "Slide";
}

void Slide::processData(char* data) {
  StaticJsonBuffer<500> buf;
  JsonObject& root = buf.parseObject(data);

  if (root.containsKey("length")) {
    this->length = root["length"];
  }

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("step")) {
    this->step = root["step"];
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

  if (this->step < 1) {
    this->step = 1;
  }

  this->strip->ClearTo(RgbColor(0,0,0));
  for(int i=0; i<this->length; i++) {
    this->strip->SetPixelColor(i, this->color);
  }
}

Slide::~Slide() {

}
