#include <Moteino.h>
#include <Button.h>

#define BTN_PIN 3

Moteino moteino;
Button btn;

void setup()
{
  moteino.setup();
  btn.init(BTN_PIN, 500);
}

unsigned long last_send=0;
unsigned long delay_ms=500;

unsigned long last_chk_btn=0;
unsigned long chk_btn_delay=500;

void loop(){
  moteino.loop();
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
        moteino.rdIdLed();
      }  else if(series[0]<=30){// 1s<push<=3s : acquire new net
        moteino.rdSearchNet();
      } else if(series[0]<=50){// 3s<push<=5s : random net and pairing
        moteino.rdRandom();
        moteino.rdPairOn();
      } else {//5s<push
      }
    } else if (series[2]==0){// two pushes : pairing
      moteino.rdPairOn();
    }else if (series[3]==0){// three pushes : stop pairing
      moteino.rdPairOff();
    }
  }
}
