#include "Twinkle.hpp"
#include "ArduinoJson.h"

Twinkle::Twinkle(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Twinkle::update(char* data) {
  this->processData(data);
}

void Twinkle::tick(unsigned long elapsed) {
  timeSinceLastRun += elapsed;
  if (timeSinceLastRun < this->delayDuration) return;
  this->timeSinceLastRun = 0;

  if (random(this->spawnRate) == 0) {
    this->strip->SetPixelColor(random(this->strip->PixelCount()-1), RgbColor(255,255,random(255)));
  }

  for(int i=0; i<strip->PixelCount(); i++) {
    RgbColor c = strip->GetPixelColor(i);
    c.Darken(this->decayRate);
    strip->SetPixelColor(i, c);
  }

  this->strip->Show();
}

const char* Twinkle::description() {
  return "Twinkle";
}

void Twinkle::processData(char* data) {
  StaticJsonBuffer<500> buf;
  JsonObject& root = buf.parseObject(data);

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("rate")) {
    this->spawnRate = 1.0 / root["rate"].as<double>();
  }

  if (root.containsKey("decay")) {
    this->decayRate = root["decay"];
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }

  if (this->decayRate < 1 || this->decayRate > 255) {
    this->decayRate = 1;
  }

  if (this->spawnRate < 1) {
    this->spawnRate = 1;
  }
}

Twinkle::~Twinkle() {

}
