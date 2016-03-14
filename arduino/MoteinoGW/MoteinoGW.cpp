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
#include <GWManager.h>

//comment to prevent adding the SerialShell
#define MOTEINO_HAS_SERIAL

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
EthShield eth;
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
  eth.init();
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

    if (strcmp(buf,"001")==0)    UpdateTeleIC((char *)moteino.radio.DATA);
    if (strcmp(buf,"2")==0)    UpdateTempInt((char *)moteino.radio.DATA);
    if (strcmp(buf,"003")==0)    UpdateMeteo((char *)moteino.radio.DATA);

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

    memset(Inst,'\0',20);
    parseMessage(Data,Inst,5);

    memset(hchp,'\0',20);
    parseMessage(Data,hchp,6);

    memset(hchc,'\0',20);
    parseMessage(Data,hchc,7);

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
  char Light[20];
  char BPV[20];
  char TV[20];
  char HV[20];
  char WD[20];
  char WS[20];
  char RV[20];

  parseMessage(Data,Light,5);
  parseMessage(Data,BPV,6);
  parseMessage(Data,TV,7);
  parseMessage(Data,HV,8);
  parseMessage(Data,WD,9);
  parseMessage(Data,WS,10);
  parseMessage(Data,RV,11);

  if (atoi(WD)>-1)
    sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field7=%s&field8=%s",BPV,TV,HV,RV,Light,WD,WS);
  else
    sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field8=%s",BPV,TV,HV,RV,Light,WS);

  updateThingSpeak(MessageServeur,writeAPIKeyMeteo);

}

// Update ThingSpeak with the date on the right chanel
void updateThingSpeak(char* tsData, char *chanel) {
  if (eth.client.connect("kgb.emn.fr", 8001)) {
    // Sending the data with a POST method
    eth.client.println(F("POST /update HTTP/1.1"));
    eth.client.print(F("Host: api.thingspeak.com\n"));
//    client.print(F("Host: kgb.emn.fr\n"));
    eth.client.print(F("Connection: close\n"));
    eth.client.print(F("X-THINGSPEAKAPIKEY: "));
    eth.client.print(chanel);
    eth.client.print(F("\n"));
    eth.client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    eth.client.print(F("Content-Length: "));
    eth.client.print(strlen(tsData));
    eth.client.print("\n\n");
    eth.client.print(tsData);
    eth.client.print("\n\n");

    eth.client.flush();
  }
  else Serial.println("PB!!! Conex");

  eth.client.stop();

  moteino.ledBlink(500);


  digitalWrite(4, HIGH);


}
