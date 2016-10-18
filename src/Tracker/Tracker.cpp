#include "Tracker.hpp"
#include "ArduinoJson.h"

Tracker::Tracker(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Tracker::update(char* data) {
  this->processData(data);
}

void Tracker::tick(unsigned long elapsed) {
  timeSinceLastRun += elapsed;
  if (timeSinceLastRun < 50) return;
  this->timeSinceLastRun = 0;

  for(int i=0; i<strip->PixelCount(); i++) {
    RgbColor c = strip->GetPixelColor(i);
    c.Darken(this->decayRate);
    strip->SetPixelColor(i, c);
  }

  this->strip->Show();
}

const char* Tracker::description() {
  return "Tracker";
}

void Tracker::processData(char* data) {
  StaticJsonBuffer<2000> buf;
  JsonObject& root = buf.parseObject(data);
  if (!root.success()) return;

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
