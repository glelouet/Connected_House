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
    Serial.print(F("received series : "));
    byte * series=btn.pop();
    for(int i=0;series[i]!=0;i++) {Serial.print(series[i]);Serial.print(" ");}
    Serial.println();
    if(series[1]==0){// one push
      if(series[0]<=10) { // <=1s push : identify elements on the network
        moteino.rdIdLed();
      }  else if(series[0]<=30){//<=3s push
        moteino.rdSNet();
      } else {//>3s push
        moteino.rdRandom();
        moteino.rdPairOn();
      }
    }
  }
}
