
#ifndef MODULELINK_H_
#define MODULELINK_H_

#include <Arduino.h>

class ModuleLink {

protected:
  virtual void doLoop(){};

  virtual void doInit(){};

  ModuleLink *next = NULL;

public:
  void loop();

  void init();

  void append(ModuleLink *element);

  int nbChildren();
};

extern ModuleLink modules;

#endif
