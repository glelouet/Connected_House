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
#include <Button.h>
#include <ButtonCommand.h>
#include <SerialShell.h>
#include <EthShield.h>

//comment to prevent adding the SerialShell
#define MOTEINO_HAS_SERIAL
#define DEBUG 1
#define DEBUG2 1

#define REPLY 1
#define ENABLE_IDLE_STBY_RF 0
#define BTN_PIN 3

#define MAX_JSON 100

char writeAPIKeyElec[] = "c33f1ab16fc77cb03b0fdc7ad11a5735";
char writeAPIKeyMeteo[] = "OD8OB7F3CWE25CLE";
char thingSpeakAddress[] = "kgb.emn.fr";
int server_port = 8001;

Moteino moteino;
Button btn;
ButtonCommand bc;
#ifdef MOTEINO_HAS_SERIAL
SerialShell sh;
#endif

char conf[35] = "{\"type\":\"Base\",\"V\":\"1\",\"id\":15}";
long int temp = 2000*30;
char buf[20];
char json[MAX_JSON];

void parseMessage(char *Message, char * Response, int value);
void UpdateTeleIC(char *Data);
void UpdateTempInt(char *Data);
void UpdateMeteo(char *Data);
void updateThingSpeak(char* tsData, char *chanel);

void setup() {
  moteino.setup();
  btn.init(BTN_PIN, 500);
  bc.init(&btn, &moteino);
  #ifdef MOTEINO_HAS_SERIAL
    sh.init(&moteino);
  #endif

  json[0]='\0';
}

void loop() {
  moteino.loop();
  bc.loop();
  #ifdef MOTEINO_HAS_SERIAL
   sh.loop();
  #endif

  if (moteino.rdRcv()) {
    if (moteino.radio.ACKRequested()) {
      moteino.radio.sendACK();
    }
    Serial.println();

    buf[0]='\0';
    parseMessage((char *)moteino.radio.DATA,buf,2);

    if (strcmp(buf,"001")==0)    UpdateTeleIC(json);
    if (strcmp(buf,"2")==0)    UpdateTempInt(json);
    if (strcmp(buf,"003")==0)    UpdateMeteo(json);

  }
}


// extract the tokenNb-th token from Message into response, where tokens are separated by ';', first token is 1
void parseMessage(char *Message, char * Response, int tokenNb) {
 // Serial.println(Message);
  int char_idx=0;
  for(;char_idx<MAX_JSON && Message[char_idx]!='\0' && tokenNb>1;char_idx++) {
    if (Message[char_idx] == ';') {
      tokenNb--;
    }
  }
  if(tokenNb==1){
    for(int cp_idx=char_idx; cp_idx<MAX_JSON && Message[cp_idx]!='\0'; cp_idx++){
      if(Message[cp_idx]==';') {
        Response[cp_idx-char_idx] ='\0';
        return;
      }
      Response[cp_idx-char_idx] = Message[cp_idx];
    }
  }
}

// Update the data for TeleIC
void UpdateTeleIC(char *Data){

  char MessageServeur[100]="";
  char Inst[20]="";
  char hchc[20]="";
  char hchp[20]="";

  memset(MessageServeur,'\0',100);

   //if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
   if (DEBUG==1) Serial.println();

    memset(Inst,'\0',20);
    parseMessage(Data,Inst,5);
    if (DEBUG==1) Serial.print("IINST :");
    if (DEBUG==1) Serial.println(Inst);

    memset(hchp,'\0',20);
    parseMessage(Data,hchp,6);
    if (DEBUG==1) Serial.print("HCHP :");
    if (DEBUG==1) Serial.println(hchp);

    memset(hchc,'\0',20);
    parseMessage(Data,hchc,7);
    if (DEBUG==1) Serial.print("HCHC :");
    if (DEBUG==1) Serial.println(hchc);

    sprintf(MessageServeur,"field4=%s&field5=%s&field6=%s",hchp,hchc,Inst);

    updateThingSpeak(MessageServeur,writeAPIKeyElec);

}

// Update the data for the temperature
void UpdateTempInt(char *Data){

  char MessageServeur[100]="";
  char TempInt[20]="";

  memset(MessageServeur,'\0',100);

  memset(TempInt,'\0',20);
  parseMessage(Data,TempInt,5);

  sprintf(MessageServeur,"field1=%s",TempInt);

  updateThingSpeak(MessageServeur,writeAPIKeyElec);

}

// Update the data for the meteo
void UpdateMeteo(char *Data){

  char MessageServeur[100]="";
   if (DEBUG==1) Serial.print("Meteo :");
   if (DEBUG==1) Serial.println(Data);
   char Light[20];
   char BPV[20];
   char TV[20];
   char HV[20];
   char WD[20];
   char WS[20];
   char RV[20];

   memset(Light,'\0',20);
   memset(BPV,'\0',20);
   memset(TV,'\0',20);
   memset(HV,'\0',20);
   memset(WD,'\0',20);
   memset(WS,'\0',20);
   memset(RV,'\0',20);

  memset(MessageServeur,'\0',100);

    parseMessage(Data,Light,5);
    if (DEBUG==1) Serial.print("Light :");
    if (DEBUG==1) Serial.println(Light);

    parseMessage(Data,BPV,6);
    if (DEBUG==1) Serial.print("BPV :");
    if (DEBUG==1) Serial.println(BPV);

    parseMessage(Data,TV,7);
    if (DEBUG==1) Serial.print("TV :");
    if (DEBUG==1) Serial.println(TV);

    parseMessage(Data,HV,8);
    if (DEBUG==1) Serial.print("HV :");
    if (DEBUG==1) Serial.println(HV);

     parseMessage(Data,WD,9);
    if (DEBUG==1) Serial.print("WD :");
    if (DEBUG==1) Serial.println(WD);

     parseMessage(Data,WS,10);
    if (DEBUG==1) Serial.print("WS :");
    if (DEBUG==1) Serial.println(WS);

    parseMessage(Data,RV,11);
    if (DEBUG==1) Serial.print("RV :");
    if (DEBUG==1) Serial.println(RV);

    if (atoi(WD)>-1)
      sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field7=%s&field8=%s",BPV,TV,HV,RV,Light,WD,WS);
    else
      sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field8=%s",BPV,TV,HV,RV,Light,WS);

    if (DEBUG==1) Serial.println(MessageServeur);
    updateThingSpeak(MessageServeur,writeAPIKeyMeteo);

}

// Update ThingSpeak with the date on the right chanel
void updateThingSpeak(char* tsData, char *chanel) {

  if (DEBUG2 == 1 ) {
    Serial.println(F("+++++++++++++++++  updateThingSpeak +++++++++++++++++++++++"));
    Serial.print("Le message: ");
    Serial.print(tsData);
    Serial.print(" Sa taille: ");
    Serial.println(strlen(tsData));
    delay(100);
  }


  //if (ENABLE_IDLE_STBY_RF==1)
  noInterrupts();
  radio.sleep();

  // Connecting to the server
  byte server[] = { 193, 54, 76, 34 }; // kgb.emn.fr
  delay(1000);
  //Serial.println(client.connect("kgb.emn.fr", 8001));
  int connectInt = client.connect("kgb.emn.fr", 8001);

  if (connectInt)
  {
    if (DEBUG==1) Serial.print("Conex");
    // Sending the data with a POST method
    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
//    client.print(F("Host: kgb.emn.fr\n"));
    client.print(F("Connection: close\n"));
    client.print(F("X-THINGSPEAKAPIKEY: "));
    client.print(chanel);
    client.print(F("\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(strlen(tsData));
    client.print("\n\n");
    client.print(tsData);
    client.print("\n\n");

   // lastConnectionTime = millis();

   client.flush();

    //  while (client.connected()) {
    //     if (client.available()) {
    //       char c = client.read();
    //       if (DEBUG==1) Serial.print(c);
    //     }
    //  }
  }
  else Serial.println("PB!!! Conex");


 client.stop();

  moteino.ledBlink(500);

//  radio.setMode(RF69_MODE_STANDBY);

  interrupts();

  digitalWrite(4, HIGH);


}
