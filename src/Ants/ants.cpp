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
  this->strip->Show();
  if (this->directionRight) {
    this->strip->RotateRight(1);
  } else {
    this->strip->RotateLeft(1);
  }
  delay(this->delayDuration);
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
  this->strip->ClearTo(RgbColor(0,0,0));
  for(int i=0; i<this->strip->PixelCount(); i++) {
    int colorIndex = i % colorCount;
    RgbColor c = RgbColor(root["colors"][colorIndex][0], root["colors"][colorIndex][1], root["colors"][colorIndex][2]);
    this->strip->SetPixelColor(i, c);
  }
}

Ants::~Ants() {
}
