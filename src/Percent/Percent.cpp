#include "Percent.hpp"
#include <ArduinoJson.h>

Percent::Percent(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data, int start, int length) {
  this->strip = strip;
  this->processData(true, data);
  this->start = start;
  this->length = length;
}

void Percent::update(char* data) {
  this->processData(false, data);
}

void Percent::tick() {

  // if we're still moving to the target state, crank the progress forward and generate new "current" values
  if (this->progress < 1) {

    this->progress += 0.01;
    if (this->progress > 1) {
      this->progress = 1;
    }

    this->percent = this->startingPercent + (this->targetPercent - this->startingPercent) * this->progress;
    this->color = RgbColor::LinearBlend(this->startingColor, this->targetColor, this->progress);
  }

  this->strip->ClearTo(RgbColor(0,0,0));
  for(int i=this->start; i<(this->start + (this->percent * this->length)); i++) {
      this->strip->SetPixelColor(i, this->color);
  }

  this->strip->Show();
  delay(this->delayDuration);
}

char* Percent::description() {
  char* description = (char*) malloc(strlen(this->name) + 7);
  sprintf(description, "%s (%d%%)", this->name, (int) (100.0*this->targetPercent));
  return description;
}

void Percent::processData(bool init, char* data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("color")) {
    this->startingColor = this->color;
    this->targetColor = RgbColor(root["color"][0], root["color"][1], root["color"][2]);
  }

  if (root.containsKey("percent")) {
    this->startingPercent = this->percent;
    this->targetPercent = root["percent"].as<float>()/100.0;
  }

  this->progress = 0;
  if (init) {
    // no need to fade from black
    this->color = this->targetColor;
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }
}

Percent::~Percent() {

}
