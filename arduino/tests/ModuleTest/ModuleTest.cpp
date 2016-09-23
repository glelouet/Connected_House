#include "ModuleTest.h"

void ModuleTest::doLoop() {
  Button btn;
  if (MButton::instance.series) {
    Serial.println(F("got a series of pushes"));
  }
}

void ModuleTest::doInit() { Serial.println(F("ModuleTest init")); }

ModuleTest ModuleTest::instance;
