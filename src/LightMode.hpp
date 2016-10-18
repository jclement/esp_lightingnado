#ifndef LIGHTMODE_H
#define LIGHTMODE_H

#include <NeoPixelBus.h>

class LightMode {
public:
  virtual ~LightMode() {};

  // called when new parameters come in for this LightMode from MQTT
  virtual void update(char* data) {};

  // called per loop.  This is responsible for updating LED strip + any required delays
  virtual void tick(unsigned long elapsed) {};

  // name for status messages
  virtual const char* description() {return NULL;}

};

#endif
