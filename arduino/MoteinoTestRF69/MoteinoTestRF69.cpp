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

char buff[10];

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
  buff[0]='\0';
  strcat(buff, "test ");
}

unsigned long last_send=0;
unsigned long delay_ms=5000;

void loop(){
  moteino.loop();
  #ifdef MOTEINO_HAS_SERIAL
  sh.loop();
  #endif
  unsigned long time = millis();
  if(moteino.rdState()==RADIO_TRANSMIT ) {
    if(time-last_send >delay_ms) {
      itoa(moteino.rdIp(), buff+4,10);
      moteino.sendBCRF69(buff);
      last_send = time;
    }
  }
  #ifdef MOTEINO_HAS_BTN
    bc.loop();
  #endif
}
