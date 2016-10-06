#include "Tracker.hpp"
#include <ArduinoJson.h>
#include "arduino.h"

Tracker::Tracker(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Tracker::update(char* data) {
  this->processData(data);
}

void Tracker::tick() {
  if (abs(millis() - this->lastRun) < 10) return;
  this->lastRun = millis();

  for(int i=0; i<strip->PixelCount(); i++) {
    RgbColor c = strip->GetPixelColor(i);
    c.Darken(this->decayRate);
    strip->SetPixelColor(i, c);
  }

  this->strip->Show();
}

char* Tracker::description() {
  char* description = (char*) malloc(strlen(this->name));
  sprintf(description, "%s", this->name);
  return description;
}

void Tracker::processData(char* data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);

  if (root.containsKey("decay")) {
    this->decayRate = root["decay"];
  }

  if (this->decayRate < 1 || this->decayRate > 255) {
    this->decayRate = 1;
  }

  if (root.containsKey("color")) {
    this->color = RgbColor(root["color"][0], root["color"][1], root["color"][2]);
  }

  for(int i=0; i<root["data"].size(); i++) {
    int pixel = (this->strip->PixelCount()-1) * root["data"][i].as<float>()/100.0;
    this->strip->SetPixelColor(pixel, this->color);
  }

}

Tracker::~Tracker() {

}
