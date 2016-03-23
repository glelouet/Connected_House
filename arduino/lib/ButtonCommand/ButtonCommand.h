#ifndef MOTEINO_BTN_CMD_H
#define MOTEINO_BTN_CMD_H

#include<Arduino.h>
#include<Moteino.h>
#include<Button.h>

class ButtonCommand{

public :

  void init(Button *button, Moteino *moteino);

  void loop();

private :

  Moteino *m;
  Button *b;

};

#endif
