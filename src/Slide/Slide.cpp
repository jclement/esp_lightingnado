#include "Slide.hpp"
#include <ArduinoJson.h>

Slide::Slide(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Slide::update(char* data) {
  this->processData(data);
}

void Slide::tick() {
  if ((millis() - this->lastRun) >= this->delayDuration) return;
  this->lastRun = millis();

  this->strip->Show();
  if (this->directionRight) {
    this->strip->RotateRight(1);
  } else {
    this->strip->RotateLeft(1);
  }
  delay(this->delayDuration);
}

char* Slide::description() {
  char* description = (char*) malloc(strlen(this->name));
  sprintf(description, "%s", this->name);
  return description;
}

void Slide::processData(char* data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);

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

  this->strip->ClearTo(RgbColor(0,0,0));
  for(int i=0; i<this->length; i++) {
    this->strip->SetPixelColor(i, this->color);
  }
}

Slide::~Slide() {

}
