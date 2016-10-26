#include "Halloween.hpp"
#include "ArduinoJson.h"

Halloween::Halloween(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, char* data) {
  this->strip = strip;
  processData(data, true);
}

void Halloween::update(char* data) {
  processData(data, false);
}

void Halloween::tick(unsigned long elapsed) {
  if (elapsed == 0) { return; } // don't bother updating the frame if no time has passed
  timeInState += elapsed;
  int currentState = state;
  // advance state machine until we've caught up
  while (timeInState >= stateTime[state]) {
    timeInState -= stateTime[state];
    if (blankBetween) {
      if (state != OFF_STATE) { state = OFF_STATE; }
      else { state = (random(NUM_STATES - 1)) + 1; } // otherwise pick one at random that is NOT the off state
    } else {
      state = (random(NUM_STATES - 1)) + 1;
    }
  }
  // yeah we could have just guessed our way back into the same state we were in, but it would be weirder if the animation reset for what appears to be no reason
  if (state != currentState) { initState(); effectLastRunTime = timeInState;} 
  updateFrame();
}

void Halloween::initState() {
//  Serial.print("initState s:");
//  Serial.println(state);
  int i;
  switch (state) { // beat and flash don't require any setup
    case SLIDERS_STATE:
      for (i = 0;i < numEffectItems;i++) {
        effectItems[i].position = ((float) random(1000) / 1000.0f);
        effectItems[i].width = 0.02f;
        effectItems[i].speed = ((float) random(100) / 1000.0f);
        effectItems[i].right = (i % 2) == 0 ? true : false;
        effectItems[i].active = true;
        int colourOffset = random(1000 / numEffectItems);
        // let's start with a random small offset then space the colours evenly around the hue wheel        
        effectItems[i].colour = RgbColor(HsbColor(((float) ((colourOffset + ((1000 * i) / numEffectItems)) % 1000) / 1000.0f), 1.0f, 0.25f));
      }      
      break;
    case TRACERS_STATE:
      for (i = 0;i < numEffectItems;i++) {
        effectItems[i].position = ((float) random(1000) / 1000.0f);
        effectItems[i].width = 0.1f;
        effectItems[i].speed = ((float) random(100) / 1000.0f);
        effectItems[i].right = (i % 2) == 0 ? true : false;
        effectItems[i].active = true;
        int colourOffset = random(1000 / numEffectItems);
        // let's start with a random small offset then space the colours evenly around the hue wheel        
        effectItems[i].colour = RgbColor(HsbColor(((float) ((colourOffset + ((1000 * i) / numEffectItems)) % 1000) / 1000.0f), 1.0f, 0.25f));
      }            
      break;
    case TWINKLE_STATE:
      // not sure what this needs yet
      break;
  }
}

void Halloween::drawSliders() {
//  Serial.print("drawSliders");
  int stripLength = strip->PixelCount();  
  float currentPos;
  int i, j;
  // advance all sliders by the elapsed time
  for (j = 0;j < numEffectItems;j++) {
    if (effectItems[j].right) {
      effectItems[j].position += (effectItems[j].speed * (timeInState - effectLastRunTime)) / 1000.0f;
    } else {
      effectItems[j].position -= (effectItems[j].speed * (timeInState - effectLastRunTime)) / 1000.0f;
    }
    if ((effectItems[j].position > (1.0f + (effectItems[j].width / 2.0f))) || 
        (effectItems[j].position < (effectItems[j].width / -2.0f))) {
      effectItems[j].position = ((float) random(1000) / 1000.0f);
      effectItems[j].width = 0.02f;
      effectItems[j].speed = ((float) random(100) / 1000.0f);
    }
  }
  // now draw them
  for (i = 0; i < stripLength;i++) {
    currentPos = ((float) i / (float) stripLength);
    for (j = 0;j < numEffectItems;j++) {
      // check if we are in the range of any effects!
      if (effectItems[j].active && (fabs(effectItems[j].position - currentPos) < effectItems[j].width)) {
        strip->SetPixelColor(i, effectItems[j].colour);
      }
    }
  }
}

const char* Halloween::description() {
  return "Halloween";
}

void Halloween::processData(char* data, bool reset) {
  StaticJsonBuffer<2000> buf;
  JsonObject& root = buf.parseObject(data);
  if (reset) { 
    state = SLIDERS_STATE;
    initState();
  }
  if (root.success()) {
    if (root.containsKey("flashLoops")) {
      flashLoops = root["flashLoops"];
      if (flashLoops < 1) {
        flashLoops = 1;
      } else if (flashLoops > 100) {
        flashLoops = 100;
      }
    }
    if (root.containsKey("beatLoops")) {
      beatLoops = root["beatLoops"];
      if (beatLoops < 1) {
        beatLoops = 1;
      } else if (beatLoops > 100) {
        beatLoops = 100;
      }
    }
    if (root.containsKey("flashTime")) {
      stateTime[FLASH_STATE] = root["flashTime"];
    }
    if (root.containsKey("beatTime")) {
      stateTime[BEAT_STATE] = root["beatTime"];
    }
    if (root.containsKey("sliderTime")) {
      stateTime[SLIDERS_STATE] = root["sliderTime"];
    }
    if (root.containsKey("tracerTime")) {
      stateTime[TRACERS_STATE] = root["tracerTime"];
    }
    if (root.containsKey("twinkleTime")) {
      stateTime[TWINKLE_STATE] = root["twinkleTime"];
    }
    if (root.containsKey("flashColour")) {
      flashColour = RgbColor(root["flashColour"][0], root["flashColour"][1], root["flashColour"][2]);
    }
    if (root.containsKey("beatColour")) {
      beatColour = RgbColor(root["beatColour"][0], root["beatColour"][1], root["beatColour"][2]);
    }
    for (int i = 0;i < NUM_STATES;i++) {
      if (stateTime[i] > 100000) {
        stateTime[i] = 100000;
      }
      if (stateTime[i] < 100) {
        stateTime[i] = 100;
      }
    }
    if (root.containsKey("blankBetween")) {
      blankBetween = root["blankBetween"];
    }
  }
  updateFrame();
}

void Halloween::blinkFullStrip(int numLoops, int arrayLength, byte* dataArray, RgbColor colour) {
//  Serial.print("blinkFullStrip(");
//  Serial.print(numLoops);
//  Serial.print(",");
//  Serial.print(arrayLength);
//  Serial.print(",[");
//  for (int i = 0;i < arrayLength;i++) {
//    Serial.print(dataArray[i]);
//    Serial.print(",");
//  }
//  Serial.print("],(");
//  Serial.print(colour.R);
//  Serial.print(",");
//  Serial.print(colour.G);
//  Serial.print(",");
//  Serial.print(colour.B);
//  Serial.println(")");
  int blockTime = stateTime[state] / (8 * numLoops * arrayLength);
  if (blockTime < 1) { blockTime = 1; }
//  Serial.print("1: ");
//  Serial.println(blockTime);
  int loopTime = stateTime[state] / numLoops;
//  Serial.print("2: ");
//  Serial.println(loopTime);
  int upperBlock = (timeInState % loopTime) / blockTime;
//  Serial.print("3: ");
//  Serial.println(upperBlock);
  int lowerBlock = upperBlock == 0 ? (arrayLength - 1) : upperBlock - 1;
//  Serial.print("4: ");
//  Serial.println(lowerBlock);
  strip->ClearTo(RgbColor::LinearBlend((dataArray[lowerBlock / 8] & (B10000000 >> (lowerBlock % 8))) ? colour : black, 
                                        (dataArray[upperBlock / 8] & (B10000000 >> (upperBlock % 8))) ? colour : black, 
                                        (float) ((timeInState % loopTime) % blockTime) / ((float) blockTime)));  
//  Serial.println("5");
}


void Halloween::drawTracers() {
  strip->SetPixelColor(state, RgbColor(0,0,255));
}

void Halloween::drawTwinkle() {
  strip->SetPixelColor(state, RgbColor(0, 255, 0));
}

void Halloween::updateFrame() {
//  Serial.print("updateFrame s:");
//  Serial.println(state);
  strip->ClearTo(RgbColor(0,0,0));
  switch (state) {
    case OFF_STATE:
      break;
    case BEAT_STATE:
      blinkFullStrip(beatLoops, beatArrayLength, beatArray, beatColour);
      break;
    case FLASH_STATE:
      blinkFullStrip(flashLoops, flashArrayLength, flashArray, flashColour);
      break;
    case SLIDERS_STATE:
      drawSliders();
      break;
    case TWINKLE_STATE:
      drawTwinkle();
      break;
    case TRACERS_STATE:
      drawTracers();
      break;
  }
  strip->Show();
//  Serial.println("E");
  effectLastRunTime = timeInState;
//  Serial.println("F");
}

Halloween::~Halloween() {

}
