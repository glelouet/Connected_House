#ifndef MODULETEST_H_
#define MODULETEST_H_

#include "Button.h"
#include "MButton.h"

class ModuleTest : public ModuleLink {

  virtual void doLoop();

  virtual void doInit();

public:
  static ModuleTest instance;

protected:
  ModuleTest() { modules.append(this); }
};

#endif
