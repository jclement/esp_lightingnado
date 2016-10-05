#include "Ants.hpp"
#include <ArduinoJson.h>

Ants::Ants(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Ants::update(char* data) {
  this->processData(data);
}

void Ants::tick() {
  if ((millis() - this->lastRun) >= this->delayDuration) return;
  this->lastRun = millis();

  this->strip->Show();
  if (this->directionRight) {
    this->strip->RotateRight(1);
  } else {
    this->strip->RotateLeft(1);
  }
}

char* Ants::description() {
  char* description = (char*) malloc(strlen(this->name));
  sprintf(description, "%s", this->name);
  return description;
}

void Ants::processData(char* data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("right")) {
    this->directionRight = root["right"];
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }

  int colorCount = root["colors"].size();
  RgbColor colors[colorCount];
  for(int i=0; i<colorCount; i++) {
    colors[i] = RgbColor(root["colors"][i][0], root["colors"][i][1], root["colors"][i][2]);
  }

  for(int i=0; i<this->strip->PixelCount(); i++) {
    this->strip->SetPixelColor(i, colors[i % colorCount]);
  }
}

Ants::~Ants() {
}
