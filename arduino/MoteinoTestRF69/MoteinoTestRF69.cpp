#include <Moteino.h>
#include <Button.h>
#include <SerialShell.h>

#define BTN_PIN 3
#define USE_SERIAL_SHELL

Moteino moteino;
Button btn;
#ifdef USE_SERIAL_SHELL
SerialShell sh;
#endif

void setup()
{
  moteino.setup();
  btn.init(BTN_PIN, 500);
  #ifdef USE_SERIAL_SHELL
  sh.init(&moteino);
  #endif
}

unsigned long last_send=0;
unsigned long delay_ms=500;

void loop(){
  moteino.loop();
  #ifdef USE_SERIAL_SHELL
  sh.loop();
  #endif
  if(moteino.rdState()==RADIO_TRANSMIT && !moteino.rdPairing()) {
    if(millis()-last_send >delay_ms) {
      moteino.sendBCRF69("test");
      last_send = millis();
    }
  }
  btn.check();
  if(btn.hasSeries()) {
    byte * series=btn.pop();
    if(series[1]==0){// one push
      if(series[0]<=10) { // push<=1s : identify elements on the network
        if(moteino.debug(DEBUG_FULL)){
          Serial.println(F("btn press led"));
        }
        moteino.rdIdLed();
      }  else if(series[0]<=30){// 1s<push<=3s : acquire new net
        if(moteino.debug(DEBUG_FULL)){
          Serial.println(F("btn acquire net"));
        }
        moteino.rdSearchNet();
      } else if(series[0]<=100){// 3s<push<=10s : random net and pairing
        if(moteino.debug(DEBUG_FULL)){
          Serial.println(F("btn rd net"));
        }
        moteino.rdRandom();
        moteino.rdPairOn();
      } else {//5s<push
      }
    } else if (series[2]==0){// two pushes : pairing
      if(moteino.debug(DEBUG_FULL)){
        Serial.println(F("btn pairon"));
      }
      moteino.rdPairOn();
    }else if (series[3]==0){// three pushes : stop pairing
      if(moteino.debug(DEBUG_FULL)){
        Serial.println(F("btn pairoff"));
      }
      moteino.rdPairOff();
    }
  }
}
