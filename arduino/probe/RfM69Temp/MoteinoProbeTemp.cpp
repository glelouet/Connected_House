#include <Moteino.h>
#include <Button.h>
#include <ButtonCommand.h>
#include <SerialShell.h>
#include <RF69Manager.h>

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

char buff[20];

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

void sendTemp(){
	uint32_t id= moteino.getId();
	uint8_t probe = 0;
	uint8_t value =moteino.radio.readTemperature();
	buff[0]='\0';
	sprintf(buff, "SEND%" PRIu32 ";%" PRIu8 ";%" PRIu8, id, probe, value);
	moteino.radio.sendGW(buff);
	Serial.println(buff);
}

void loop(){
  moteino.loop();
  #ifdef MOTEINO_HAS_SERIAL
  sh.loop();
  #endif
  unsigned long time = millis();
  if(moteino.radio.getState()==rfm69::TRANSMIT && time-last_send >moteino.params.probePeriod) {
		sendTemp();
    last_send = time;
  }
  #ifdef MOTEINO_HAS_BTN
    bc.loop();
  #endif
}
