#include <Moteino.h>

#undef DEBUG
#define DEBUG 1

Moteino moteino;

void setup()
{
  moteino.acquire_RF69_IP = false;
	moteino.setup();
}

unsigned long last_send=0;
unsigned long delay_ms=500;

void loop(){
  moteino.loop();
  if(moteino.radio_state==RADIO_TRANSMIT && !moteino.pairing) {
    if(millis()-last_send >delay_ms) {
      moteino.sendBCRF69("test");
      last_send = millis();
    }
  }
}
