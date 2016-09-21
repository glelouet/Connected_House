
#ifndef MODULELINK_H_
#define MODULELINK_H_

#include <Arduino.h>

class ModuleLink;

class ModuleLink {

protected:
  ModuleLink *next = NULL;

  virtual void doLoop(){};

  virtual void doInit(){};

public:
  void loop();

  void init();

  void append(ModuleLink *element);
};

extern ModuleLink modules;

#endif
