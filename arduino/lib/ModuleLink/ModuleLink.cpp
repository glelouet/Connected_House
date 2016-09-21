#include "ModuleLink.h"

void ModuleLink::loop() {
  doLoop();
  if (next)
    next->loop();
}

void ModuleLink::init() {
  doInit();
  if (next)
    next->init();
}

void setup() { modules.init(); }

void loop() { modules.loop(); }

ModuleLink modules;
