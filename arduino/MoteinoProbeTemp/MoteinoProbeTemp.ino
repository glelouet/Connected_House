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

//#include <SPI.h>

#include <Moteino.h>

#define DEBUG 1
#define DISTRIBUTED_DEBUG     0
#define VERSION 001
#define ACK_TIME    30  // # of ms to wait for an ack
//#define ENCRYPTKEY "sampleEncryptKey" //(16 bytes of your choice - keep the same on all encrypted nodes)


long TRANSMITPERIOD = 2000; //transmit a packet to gateway so often (in ms)
long lastPeriod = 0;
char input = 0;

Moteino moteino;

void setup()
{
	moteino.setup();
}

void parseMessage(char *Message, char * Response, int value) {
  int cpt = 0;
 // Serial.println(Message);

  for (int i=0; (i<100) & (i<(strlen(Message) ) ); ) {
    if (cpt==value) {
      for (int j=0; (Message[i]!=';' & (i<100) & (i<(strlen(Message) ) ) ); ) {
     //  Serial.print(cpt);Serial.print('-');Serial.print(i);Serial.print('-');Serial.print(j);

        Response[j] = Message[i];
        j++;
        i++;
      }
      return ;
    }

    if (Message[i] == ';') {
      cpt++;
    }
    i++;

  }
}

// [SousReseau,To,From,Type,Version,...]
// [NETWORKID;GATEWAY;NODEID;TYPE;VERSION...]

void sendTemp(){
  char temp1[20];


  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    lastPeriod=currPeriod;
  if (DEBUG==1) Serial.println("Loop");

  float temp;
  char buf2[20];

      // Lit la température ambiante à ~1Hz
      //if(getTemperature(&temp)) {
     temp = -10;
     if (DEBUG ==1 ) {
      // Affiche la température
      Serial.print("Temperature : ");
      Serial.print(temp);
      Serial.write(' '); // caractère °
      Serial.write('C');
      Serial.println();
     }
  //}
  //delay(500);



  Serial.println("Data Serveur:");
  moteino.ftoa(temp,buf2,0);
  Serial.print("message is");
  Serial.print(buf2);
  Serial.println();
  moteino.sendToGateway(buf2);
  }

}



void loop()
{
  // Check for existing RF data, potentially for a new sketch wireless upload
  // For this to work this check has to be done often enough to be
  // picked up when a GATEWAY is trying hard to reach this node for a new sketch wireless upload

  if (moteino.radio.receiveDone())
  {
    Serial.print("Got [");
    Serial.print(moteino.radio.SENDERID);
    Serial.print(':');
    Serial.print(moteino.radio.DATALEN);
    Serial.print("] > ");
    for (byte i = 0; i < moteino.radio.DATALEN; i++)
      Serial.print((char)moteino.radio.DATA[i], HEX);
    Serial.println();
    CheckForWirelessHEX(moteino.radio, moteino.flash, true);
    Serial.println();
  } else {

  ////////////////////////////////////////////////////////////////////////////////////////////
  // Normal sketch code here
   sendTemp();
  ////////////////////////////////////////////////////////////////////////////////////////////
  }
}
