// rf69_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <Moteino.h>

long TRANSMITPERIOD = 2000; //transmit a packet to gateway so often (in ms)
long lastPeriod = 0;

Moteino moteino;

void setup()
{
	moteino.setup();
}

void sendTemp(){
  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    lastPeriod=currPeriod;
  	if (DEBUG) Serial.println("Loop");
		float temp;
  	char buf2[20];
    // Lit la température ambiante à ~1Hz
    //if(getTemperature(&temp)) {
   temp = -10;
   if (DEBUG) {
    // Affiche la température
      Serial.print("Temperature : ");
      Serial.print(temp);
      Serial.write(' '); // caractère °
      Serial.write('C');
      Serial.println();
    }
  	//}
  	//delay(500);
		moteino.ftoa(temp,buf2,0);
  	Serial.println("sending to gateway : ");  Serial.print(buf2);
  	Serial.println();
		char trame[100];
		memset(trame,'\0',100);
		sprintf(trame,"%lu;%c%c%c;",moteino.flashId,VERSION[0],VERSION[1],VERSION[2]);
		  strcat(trame, buf2);
  	moteino.sendRF69(moteino.params.gwId, trame);
  }
}



void loop()
{
  // Check for existing RF data, potentially for a new sketch wireless upload
  // For this to work this check has to be done often enough to be
  // picked up when a GATEWAY is trying hard to reach this node for a new sketch wireless upload
	moteino.loop();

  ////////////////////////////////////////////////////////////////////////////////////////////
  // Normal sketch code here
   sendTemp();
  ///////////////////////////////////////////////////////////////////////////////////////////
}
