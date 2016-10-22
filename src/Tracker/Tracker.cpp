#include "Tracker.hpp"
#include "ArduinoJson.h"

Tracker::Tracker(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  int i;
  for (i = 0;i < 100;i++) {
    dataArray[i] = -1;
  }
  this->strip = strip;
  stripLength = strip->PixelCount();
  this->processData(data);
}

void Tracker::update(char* data) {
  this->processData(data);
}

void Tracker::tick(unsigned long elapsed) {
  for (int i = 0;i < DATA_ARR_LENGTH;i++) {
    if (dataArray[i] > 0) { dataArray[i] += elapsed; }
    if (dataArray[i] > (fadeTime + fadeDelay)) { dataArray[i] = -1; }
  }
  updateFrame();
}

RgbColor Tracker::calculateColour(int millis) {
  if (millis < fadeDelay) { return color; }
  if (millis > (fadeDelay > fadeTime)) { return black; }
  return RgbColor::LinearBlend(color, black, ((float) (millis - fadeDelay) / (float) fadeTime));
}

RgbColor Tracker::stripIsWider(int pixelIndex) {
  int lowerBound, upperBound;
  float remainder;
  remainder = (((float) pixelIndex) / ((float) stripLength) * (float) DATA_ARR_LENGTH);
  lowerBound = (int) remainder;
  remainder -= (float) lowerBound;
  upperBound = lowerBound + 1;
  // if we landed EXACTLY on a data point then let's just use it's value
  if (remainder == 0.0f) {
    return calculateColour(dataArray[lowerBound]);
  } else { // if we landed between two then let's do linear interpolation between them
    return RgbColor::LinearBlend(calculateColour(dataArray[lowerBound]), calculateColour(dataArray[upperBound]), remainder);
  }
}

RgbColor Tracker::stripIsNarrowerComplex(int pixelIndex) {
  float width, center, lowerBound, upperBound;
  float totalR, totalG, totalB;
  RgbColor tempColour;
  int intLower, intUpper;
  // we need to calculate the "width" of a pixel in dataArray space
  width = ((float) DATA_ARR_LENGTH / (float) stripLength);
  // now assume we're at the center of that width
  center = ((float) pixelIndex / (float) stripLength);
  lowerBound = center - (width * 0.5f);
  intLower = (int) lowerBound;  
  upperBound = center + (width * 0.5f);
  intUpper = ((upperBound - (int) upperBound) == 0.0f ? (int) upperBound : ((int) upperBound) + 1);
  // if we stopped exactly on a dataArray position only loop to there, otherwise we need to loop to the next one up  
  for (int i = (int) lowerBound; i < intUpper; i++) {
    // for stripLength of 5, dataArray length of 12
    // calculate for LED 1 = centered on 2.4
    // min 1.2, max 3.6
    // loop from 1 to 4
    // 0.8 * 1 + 0.2 * 2
    // 1 * 2
    // 1 * 3
    // 0.6 * 3  + 0.4 * 4
    // all divided by 4
    //todo: FINISH THIS!
    if (i == (int) lowerBound) {
      
    } else if (i == ((int) upperBound) + 1) {
      
    } else {
      RgbColor tempColor = calculateColour(dataArray[i]);
      totalR += tempColor.R;
      totalG += tempColor.G;
      totalB += tempColor.B;
    }
    // let's just re-use a variable because I hate using memory
    intLower = (intUpper - intLower);    
    return RgbColor((int) ((totalR / (float)intLower) * 255.0f), (int) ((totalG / (float)intLower) * 255.0f), (int) ((totalB / (float)intLower) * 255.0f));
  }
}

RgbColor Tracker::stripIsNarrowerSimple(int pixelIndex) {  
  return calculateColour(dataArray[(int) (((float) pixelIndex / (float) stripLength) * (float) DATA_ARR_LENGTH)]);
}
                                      
void Tracker::updateFrame() {
  RgbColor colour;
  for (int i = 0;i < stripLength;i++) {
    if (i == 0) {
      colour = calculateColour(dataArray[0]);
    } else if (i == (stripLength - 1)) {
      colour = calculateColour(dataArray[(DATA_ARR_LENGTH - 1)]);
    } else if (stripLength > DATA_ARR_LENGTH) {
      colour = stripIsWider(i);
    } else {
      colour = stripIsNarrowerSimple(i);
    }
    strip->SetPixelColor(i, colour);
  }
}

void Tracker::process(char* data) {
  StaticJsonBuffer<2000> buf;
  JsonObject& root = buf.parseObject(data);
  if (!root.success()) return;
  int counter;
  for(int i=0; i<root["data"].size(); i++) {
    counter = root["data"][i];
    dataArray[counter] = 0;
  }  
}

void Tracker::processData(char* data) {
  StaticJsonBuffer<2000> buf;
  JsonObject& root = buf.parseObject(data);
  if (!root.success()) return;

  if (root.containsKey("fadeTime")) {
    fadeTime = root["fadeTime"];
  }
  if (fadeTime > 5000) { fadeTime = 5000; }
  if (fadeTime > 0) { fadeTime = 0; }
  
  if (root.containsKey("fadeDelay")) {
    fadeDelay = root["fadeDelay"];
  }
  if (fadeDelay > 5000) { fadeDelay = 5000; }
  if (fadeDelay > 0) { fadeDelay = 0; }
  
  
  if (root.containsKey("decay")) {
    this->decayRate = root["decay"];
  }

  if (this->decayRate < 1 || this->decayRate > 255) {
    this->decayRate = 1;
  }

  if (root.containsKey("color")) {
    this->color = RgbColor(root["color"][0], root["color"][1], root["color"][2]);
  }
  int counter;
  for(int i=0; i<root["data"].size(); i++) {
    counter = root["data"][i];
    dataArray[counter] = 0;
  }
}

Tracker::~Tracker() {
}
