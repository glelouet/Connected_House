#include <SerialShell.h>

void SerialShell::init(Moteino *moteino){
  m = moteino;
}

void SerialShell::loop(){
  while(Serial.available()>0) {
    char cread = Serial.read();
    serial_buffer[serial_blength++]=cread;
    if(cread=='\n' || cread=='\r'){
      Serial.println();
      serial_buffer[serial_blength-1]='\0';
      if(serial_blength>1) handleSerialMessage(serial_buffer);
      serial_blength=0;
    } else if(cread==127 || cread==8){//backspace or del
      serial_blength-=2;
      if(serial_blength<0) serial_blength=0;
    } else {
      Serial.print(cread);
    }
    if(serial_blength>=SERIAL_BUFFER_SIZE) {
      serial_buffer[SERIAL_BUFFER_SIZE-1]='\0';
      Serial.println(F("error : serial buffer overflow, discarding serial buffer :"));
      Serial.println(serial_buffer);
      serial_blength=0;
    }
  }
}

void SerialShell::printParams(){
  Serial.print(F("version="));Serial.println(m->params.version);
  Serial.print(F("debug="));Serial.println(m->params.debug);
  Serial.print(F("paired="));Serial.println(m->params.paired);
  Serial.print(F("rdNet="));Serial.println(m->params.rdNet);
  Serial.print(F("rdIP="));Serial.println(m->params.rdIP);
  Serial.print(F("rdKey="));
  for(int i=0;i<RF69_CRYPT_SIZE;i++) {
    Serial.print(' ');
    Serial.print((byte)m->params.rdKey[i]);
  } Serial.println();
  Serial.print(F("ethMac="));
  for(int i=0;i<ETH_MAC_SIZE;i++) {
    Serial.print(' ');
    Serial.print(m->params.ethMac[i]);
  } Serial.println();
}

void SerialShell::handleSerialMessage(char *message) {
  if(strncmp(message, "rdnet=", strlen("rdnet="))==0) {
    uint8_t rdNet = atoi(message+strlen("rdnet="));
    Serial.print(F("set rdnet "));
    Serial.println(rdNet);
    m->rdSetNet(rdNet);
  } else if(strcmp(message, "rdSearchNet")==0) {
    Serial.println(F("search rd net"));
    m->rdSearchNet();
  } else if(strncmp(message, "rdip=", strlen("rdip="))==0) {
    uint8_t netIP = atoi(message+strlen("rdip="));
    Serial.print(F("set rdip "));
    Serial.println(netIP);
    m->rdSetIP(netIP);
  } else if(strcmp(message, "rdSearchIP")==0) {
    Serial.println(F("acquiring radio IP"));
    m->rdSearchIP();
  } else if(strcmp(message, "rdran")==0) {
    Serial.println(F("ran rd data"));
    m->rdRandom();
  } else if(strcmp(message, "write")==0) {
    m->writeEEPROM();
    Serial.println(F("wr2EEPROM ok"));
  } else if(strcmp(message, "load")==0) {
    m->loadEEPROM();
    Serial.println(F("loadEEPROM"));
    printParams();
  } else if(strcmp(message, "params")==0) {
    printParams();
  } else if(strncmp(message, "dbg=", strlen("dbg="))==0) {
    int debug = atoi(message+strlen("dbg="));
    Serial.print(F("set dbg "));
    Serial.println(debug);
    m->params.debug=debug;
  } else if(strcmp(message, "pairon")==0) {
    m->rdPairOn();
  } else if(strcmp(message, "pairoff")==0) {
    m->rdPairOff();
  } else if(strncmp(message, "blink ", strlen("blink "))==0) {
    int time = atoi(message+strlen("blink "));
    m->ledBlink(time);
  } else if(strncmp(message, "flash ", strlen("flash "))==0) {
    int time = atoi(message+strlen("flash "));
    m->ledFlash(time);
  } else {
    Serial.print(F("discarding "));
    Serial.println(message);
  }
}
