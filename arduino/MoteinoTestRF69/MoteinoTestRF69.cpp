#include <Moteino.h>
#include <Button.h>
#include <ButtonCommand.h>
#include <SerialShell.h>

#define BTN_PIN 3
#define MOTEINO_HAS_BTN
#define MOTEINO_HAS_SERIAL

//uncomment to prevent the include of serial and button-related code
//#define MOTEINO_NO_COMMAND


#ifdef MOTEINO_NO_COMMAND
#undef MOTEINO_HAS_BTN
#undef MOTEINO_HAS_SERIAL
#endif

Moteino moteino;
#ifdef MOTEINO_HAS_BTN
Button btn;
ButtonCommand bc;
#endif
#ifdef MOTEINO_HAS_SERIAL
SerialShell sh;
#endif

void setup()
{
  moteino.setup();
  #ifdef MOTEINO_HAS_BTN
  btn.init(BTN_PIN, 500);
  bc.init(&btn, &moteino);
  #else
  moteino.rdSearchNet();
  #endif
  #ifdef MOTEINO_HAS_SERIAL
  sh.init(&moteino);
  #endif
}

unsigned long last_send=0;
unsigned long delay_ms=500;

void loop(){
  moteino.loop();
  #ifdef MOTEINO_HAS_SERIAL
  sh.loop();
  #endif
  if(moteino.rdState()==RADIO_TRANSMIT && !moteino.rdPairing()) {
    if(millis()-last_send >delay_ms) {
      moteino.sendBCRF69("test");
      last_send = millis();
    }
  }
  #ifdef MOTEINO_HAS_BTN
    bc.loop();
  #endif
}
