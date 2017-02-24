#include<ButtonCommand.h>

void ButtonCommand::init(Button *button, Moteino *moteino){
  b=button;
  m=moteino;
}

void ButtonCommand::loop() {
  b->check();
  if(b->hasSeries()) {
    byte * series=b->pop();
//    Serial.print(F("acquired button series"));
//    for(int i=0;series[i]!=0;i++) {
//      Serial.print(' ');Serial.print(series[i], DEC);
//    }
//    Serial.println();
    if(series[1]==0){// one push
      if(series[0]<=5) { // push<=0.5s : identify elements on the network
        m->rdIdLed();
      }  else if(series[0]<=30){// 0.5s<push<=3s : acquire new net
        m->radio.searchNet();
      } else if(series[0]<=100){// 3s<push<=10s : random net and pairing
        m->rdRandom();
        m->radio.pair();
      } else {//10s<push
      }
    } else if (series[2]==0){// two pushes : pairing
      m->radio.pair();
    }else if (series[3]==0){// three pushes : stop pairing
      m->radio.pair(false);
    }
  }
}
