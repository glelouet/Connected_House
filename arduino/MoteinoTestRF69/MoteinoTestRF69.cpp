#include <Moteino.h>

#undef DEBUG
#define DEBUG 1

Moteino moteino;

void setup()
{
  moteino.acquire_from_EEPROM = false;
  moteino.acquire_RF69_infos = false;
	moteino.setup();
  Serial.println("setup!");
}

void loop(){
  moteino.loop();
}
