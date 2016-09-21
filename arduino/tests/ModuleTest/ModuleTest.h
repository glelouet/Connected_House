#ifndef MODULETEST_H_
#define MODULETEST_H_

#include "ModuleLink.h"

class ModuleTest : public ModuleLink {

  static ModuleTest instance;

public:
  ModuleTest() { modules.append(this); }

protected:
  virtual void doLoop();

  virtual void doInit();
  
};

#endif

