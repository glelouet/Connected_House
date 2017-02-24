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

void setup() {
  Serial.begin(9600);
  modules.init();

  Serial.print(F("setup with "));
  Serial.print(modules.nbChildren());
  Serial.println(F(" modules"));
}

void loop() { modules.loop(); }

void ModuleLink::append(ModuleLink *element) {
  if (next)
    next->append(element);
  else
    next = element;
}

int ModuleLink::nbChildren() {
  if (next)
    return next->nbChildren() + 1;
  return 0;
}

ModuleLink modules;
