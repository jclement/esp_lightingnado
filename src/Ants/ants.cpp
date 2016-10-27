#include "Ants.hpp"
#include "ArduinoJson.h"

Ants::Ants(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Ants::update(char* data) {
  delete [] colours;
  this->processData(data);
}

void Ants::tick(unsigned long elapsed) {
  accumulatedTime += elapsed;
  if (accumulatedTime < this->delayDuration) return;
  while (accumulatedTime >= this->delayDuration) {
    if (this->directionRight) {
      lastStart--;
    } else {
      lastStart++;
    }
    if (lastStart >= numColours) {
      lastStart = lastStart - numColours;
    }
    if (lastStart < 0) {
      lastStart = numColours + lastStart;
    }
    accumulatedTime -= this->delayDuration;
  }
  for(int i=0; i<this->strip->PixelCount(); i++) {
    this->strip->SetPixelColor(i, colours[(i + lastStart) % numColours]);
  }
  this->strip->Show();
}

const char* Ants::description() {
  return "Ants";
}

void Ants::processData(char* data) {
  StaticJsonBuffer<500> buf;
  JsonObject& root = buf.parseObject(data);

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("right")) {
    this->directionRight = root["right"];
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }

  int oldNumColours = numColours;
  numColours = root["colors"].size();
  if (oldNumColours != numColours) {
    if (colours != NULL) {
      delete[] colours;
    }
    colours = new RgbColor[numColours];
  }
  for(int i=0; i<numColours; i++) {
    colours[i] = RgbColor(root["colors"][i][0], root["colors"][i][1], root["colors"][i][2]);
  }
  
  for(int i=0; i<this->strip->PixelCount(); i++) {
    this->strip->SetPixelColor(i, colours[i % numColours]);
  }
}

Ants::~Ants() {
  delete [] colours;
}
