#include "Rainbow.hpp"
#include "ArduinoJson.h"

RgbColor wheel(uint8_t position)
{
	position = 255 - position;
	if (position < 85) {
		return  RgbColor(255 - position * 3, 0, position * 3);
	} else if (position < 170) {
		position -= 85;
		return RgbColor(0, position * 3, 255 - position * 3);
	} else {
		position -= 170;
		return  RgbColor(position * 3, 255 - position * 3, 0);
	}
}

Rainbow::Rainbow(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  this->processData(data);
}

void Rainbow::update(char* data) {
  this->processData(data);
}

void Rainbow::tick() {
  if (abs(millis() - this->lastRun) < this->delayDuration) return;
  this->lastRun = millis();

  if (this->directionRight) {
    this->strip->RotateRight(this->step);
  } else {
    this->strip->RotateLeft(this->step);
  }
  this->strip->Show();
}

const char* Rainbow::description() {
  return "Rainbow";
}

void Rainbow::processData(char* data) {
	StaticJsonBuffer<500> buf;
	JsonObject& root = buf.parseObject(data);

  if (root.containsKey("delay")) {
    this->delayDuration = root["delay"];
  }

  if (root.containsKey("brightness")) {
    this->brightness = root["brightness"].as<float>() / 100.0;
  }

  if (root.containsKey("step")) {
    this->brightness = root["step"].as<int>();
  }

  if (root.containsKey("right")) {
    this->directionRight = root["right"];
  }

  if (this->delayDuration < 1) {
    this->delayDuration = 1;
  }

  if (this->step < 1) {
    this->step = 1;
  }

  if (this->brightness < 0) {
    this->brightness = 0;
  }

  if (this->brightness > 1) {
    this->brightness = 1;
  }

  for(int i=0; i<this->strip->PixelCount(); i++) {
    RgbColor color = wheel(255 * i / this->strip->PixelCount());
    this->strip->SetPixelColor(i, RgbColor::LinearBlend(RgbColor(0,0,0), color, this->brightness));
  }

}

Rainbow::~Rainbow() {

}
