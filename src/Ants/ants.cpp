#include "Ants.hpp"
#include "ArduinoJson.h"

Ants::Ants(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Ants::update(char* data) {
  this->processData(data);
}

void Ants::tick(unsigned long elapsed) {
  timeSinceLastRun += elapsed;
  if (timeSinceLastRun < this->delayDuration) return;
  this->timeSinceLastRun = 0;
  if (this->directionRight) {
    lastStart++;
  } else {
    lastStart--;
  }
  if (lastStart >= numColours) {
    lastStart = lastStart % numColours;
  }
  if (lastStart < 0) {
    lastStart = numColours + lastStart;
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

  numColours = root["colors"].size();
  colours = new RgbColor[numColours];
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
