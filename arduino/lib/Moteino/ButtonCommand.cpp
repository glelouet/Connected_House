#include<ButtonCommand.h>

void ButtonCommand::init(Button *button, Moteino *moteino){
  b=button;
  m=moteino;
}

void ButtonCommand::loop() {
  b->check();
  if(b->hasSeries()) {
    byte * series=b->pop();
    if(series[1]==0){// one push
      if(series[0]<=5) { // push<=0.5s : identify elements on the network
        if(m->debug(DEBUG_FULL)){
          Serial.println(F("btn press led"));
        }
        m->rdIdLed();
      }  else if(series[0]<=30){// 0.5s<push<=3s : acquire new net
        if(m->debug(DEBUG_FULL)){
          Serial.println(F("btn acquire net"));
        }
        m->rdSearchNet();
      } else if(series[0]<=100){// 3s<push<=10s : random net and pairing
        if(m->debug(DEBUG_FULL)){
          Serial.println(F("btn rd net"));
        }
        m->rdRandom();
        m->rdPairOn();
      } else {//10s<push
      }
    } else if (series[2]==0){// two pushes : pairing
      if(m->debug(DEBUG_FULL)){
        Serial.println(F("btn pairon"));
      }
      m->rdPairOn();
    }else if (series[3]==0){// three pushes : stop pairing
      if(m->debug(DEBUG_FULL)){
        Serial.println(F("btn pairoff"));
      }
      m->rdPairOff();
    }
  }
}
