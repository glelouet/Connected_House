// rf69_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_client
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration.
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <Moteino.h>
#include <SerialShell.h>
#include <W5100Manager.h>

// comment to prevent adding the SerialShell
#define MOTEINO_HAS_SERIAL

char writeAPIKeyElec[] = "c33f1ab16fc77cb03b0fdc7ad11a5735";
char writeAPIKeyMeteo[] = "OD8OB7F3CWE25CLE";

Moteino moteino;
W5100Manager gw;
#ifdef MOTEINO_HAS_SERIAL
SerialShell sh;
#endif

void parseMessage(char *Message, char *Response, int value);
void updateThingSpeak(char *tsData, char *chanel);

void setup() {
  moteino.setup();
  gw.init(&moteino, "tarzan.info.emn.fr", 80);
#ifdef MOTEINO_HAS_SERIAL
  sh.init(&moteino);
#endif
  moteino.radio.setIP(moteino.radio.GWIP);
}

void loop() {
  moteino.loop();
#ifdef MOTEINO_HAS_SERIAL
  sh.loop();
#endif
  gw.loop();
}
