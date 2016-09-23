#ifndef MBUTTON_H
#define MBUTTON_H

#include "Button.h"
#include "ModuleLink.h"

// a buton module. When loaded, get the pushes on the button on each loop
// this module only reads on pin 3 with a 500ms end for each series
class MButton : public ModuleLink {

  virtual void doLoop();

  virtual void doInit();

public:
  byte *series;

  static MButton instance;

protected:
  Button button;

  MButton() { modules.append(this); }
};

#endif
