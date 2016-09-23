#include "MButton.h"

void MButton::doLoop() {
  this.button.check();
  series = 0;
  if (button.hasSeries())
    series = button.pop();
}

void MButton::doInit() { this.button.init(3, 500); }

MButton MButton::instance;
