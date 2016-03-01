#include <Moteino.h>

#undef DEBUG
#define DEBUG 1

Moteino moteino;

void setup()
{
  moteino.acquire_RF69_IP = false;
	moteino.setup();
  Serial.println("setup!");
}

unsigned long last_send=0;
unsigned long delay_ms=5000;

void loop(){
  moteino.loop();
  if(millis()-last_send >delay_ms) {
    last_send = millis();
    moteino.blink(100);
    moteino.sendBCRF69("test");
    Serial.print("sending test at time ");
    Serial.println(last_send);
    moteino.blink(100);
  }
}
