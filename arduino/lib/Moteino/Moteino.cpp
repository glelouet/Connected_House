/*
 * MOTEINO.cpp
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#include <Moteino.h>
#include <Arduino.h>

void Moteino::blink(int DELAY_MS)
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(LED_PIN,LOW);
}

Moteino::Moteino():
  owire(ONEWIRE_PIN),
	flash(FLASH_PIN, 0xEF30)//0xEF30 for windbond 4mbit flash
{
}

Moteino::~Moteino(){
}

void Moteino::setup(){
  Serial.begin(SERIAL_BAUD);
  init_EEPROM();
  pinMode(LED_PIN, OUTPUT);

  // this order is important :
  // flash gives us its unique id so we can translate it to a *mac* address for the RF69
  // if ethernet is present then we are a gateway, so our IP on the RF69 network is 0
  // we then initialize the RF69 using those parameters
  init_flash();
  init_ethernet();
  init_RF69();
  if(rewrite_EEPROM) writeEEPROM();
}

void Moteino::check_serial(){
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
      Serial.println("error : serial buffer overflow, discarding serial buffer :");
      Serial.println(serial_buffer);
      serial_blength=0;
    }
  }
}

void Moteino::printParams(){
  Serial.print("version=");Serial.println(params.version);
  Serial.print("netStored=");Serial.println(params.netStored);
  Serial.print("nodeId=");Serial.println(params.nodeId);
  Serial.print("gwId=");Serial.println(params.gwId);
  Serial.print("netWord=");Serial.println(params.netWord);
  Serial.print("encrypt=");Serial.println(params.encrypt);
}

void Moteino::handleSerialMessage(char *message) {
  if(strncmp(message, "netword=", strlen("netword="))==0) {
    int netword = atoi(message+strlen("netword="));
    Serial.print("setting net word to ");
    Serial.println(netword);
    params.netWord=netword;
    radio.setNetwork(netword);
  } else if(strcmp(message, "write")==0) {
    writeEEPROM();
    Serial.println("writing params to EEPROM");
  } else if(strcmp(message, "load")==0) {
    loadEEPROM();
    Serial.println("loaded params from EEPROM :");
    printParams();
  } else if(strcmp(message, "params")==0) {
    printParams();
  }else if(strcmp(message, "pairon")==0) {
    pairOn();
  } else if(strcmp(message, "pairoff")==0) {
    pairOff();
  } else if(strncmp(message, "blink ", strlen("blink "))==0) {
    int time = atoi(message+strlen("blink "));
    blink(time);
  } else {
    Serial.print("discarding ");
    Serial.println(message);
  }
}

void Moteino::init_EEPROM(){
  if(acquire_from_EEPROM && !loadEEPROM())
  writeEEPROM();
}

boolean Moteino::loadEEPROM(){
    if(DEBUG) Serial.println("acquiring parameters from EEPROM");
    // To make sure there are settings, and they are YOURS!
    // If nothing is found it will use the default settings.
    if (EEPROM.read(EEPROM_offset + 0) == VERSION[0]
      && EEPROM.read(EEPROM_offset + 1) == VERSION[1]
      && EEPROM.read(EEPROM_offset + 2) == VERSION[2]){
        for (unsigned int t=0; t<sizeof(params); t++)
          *((char*)&params + t) = EEPROM.read(EEPROM_offset + t);
    } else {
      if(DEBUG) Serial.println("incorrect version in EEPROM");
      return false;
    }
  return true;
}



void Moteino::writeEEPROM() {
  for (unsigned int t=0; t<sizeof(params); t++)
    EEPROM.update(EEPROM_offset + t, *((char*)&params + t));
}

void Moteino::init_flash(){
  if (flash.initialize()) {
    uint8_t* uniq_id = flash.readUniqueId();
    flashId=0;
    for (byte i=4;i<8;i++) {
      flashId=flashId<<8|uniq_id[i];
    }
    if(DEBUG) {
      Serial.print("flashId=");
      Serial.println(flashId);
    }
  }
  else
    Serial.println("SPI Flash Init FAIL!");
}

void Moteino::init_ethernet(){
  hasEthernet=digitalRead(ETHERNET_PIN);
  if(DEBUG) {
    Serial.print("ethernet presence : ");
    Serial.println(hasEthernet);
  }
  if(hasEthernet) {
      Serial.println("Start Ethernet");
      ethc.stop();

      if (DEBUG) Serial.println(F("Connecting Arduino to network..."));

      delay(4000);
      // Change the PIN used for the ethernet
      Ethernet.select(ETHERNET_PIN);


      // Connect to network amd obtain an IP address using DHCP
      if (Ethernet.begin(params.ethMac) == 0)
      {
        if (DEBUG) Serial.println(F("DHCP Failed, reset Arduino to try again"));
      }
      else
      {
        if (DEBUG) Serial.println(F("Arduino connected to network using DHCP"));
      }

      if (DEBUG) {
        Serial.print("My IP address: ");
        for (byte thisByte = 0; thisByte < 4; thisByte++) {
          // print the value of each byte of the IP address:
          Serial.print(Ethernet.localIP()[thisByte], DEC);
          Serial.print(".");
        }
        Serial.println();
      }
      W5100.setRetransmissionTime(0x07D0); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
      W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.

      delay(100);
  }
}

///////////////////////////////////////////////////////////
// RF69
///////////////////////////////////////////////////////////

void Moteino::init_RF69(){
  if(hasEthernet) params.nodeId=gw_RF69;
  radio.initialize(RF69_433MHZ,params.nodeId,params.netWord);
  //radio.encrypt(ENCRYPTKEY); //OPTIONAL
  #ifdef IS_RFM69HW //only for RFM69HW
    radio.setHighPower();!
  #endif
  radio.enableAutoPower(-60);
  if(acquire_RF69_infos) {

  } else {
    // the info are already retrieved from the EEPROM
  }
}

void Moteino::pairOn(){

}

void Moteino::pairOff(){

}

void Moteino::sendRF69(char *trame, byte targetId){
  int sendSize = strlen(trame);
  // Send a message to rf69_server
  uint8_t data[100];
  for (int i=0;i<100;i++)
    data[i]=trame[i];
  blink(3);
  radio.sendWithRetry(targetId, data, sendSize);
}

void Moteino::sendBCRF69(char *data){
  radio.send(RF69_BROADCAST_ADDR, data, strlen(data));
}

void Moteino::check_RF69(){
  if (radio.receiveDone()) {
    if(strcmp("test", (char *)radio.DATA)==0) {
      Serial.println("received test on RF69");
    }else
    // check if radio received rom to write on the flash, then flash it
    CheckForWirelessHEX(radio, flash, true);
  }
}

/////////////////////////////////////////////////
// DS18B20
/////////////////////////////////////////////////

// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
boolean Moteino::getTemperatureDS18B20(float *temp){
	byte data[9], addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté

  if (!owire.search(addr)) { // Recherche un module 1-Wire
    owire.reset_search();    // Réinitialise la recherche de module
    return false;         // Retourne une erreur
  }

  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
    return false;                        // Si le message est corrompu on retourne une erreur

  if (addr[0] != DS18B20_PIN) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur

  owire.reset();             // On reset le bus 1-Wire
  owire.select(addr);        // On sélectionne le DS18B20

  owire.write(0x44, 1);      // On lance une prise de mesure de température
  delay(800);             // Et on attend la fin de la mesure

  owire.reset();             // On reset le bus 1-Wire
  owire.select(addr);        // On sélectionne le DS18B20
  owire.write(0xBE);         // On envoie une demande de lecture du scratchpad

  for (byte i = 0; i < 9; i++) // On lit le scratchpad
    data[i] = owire.read();       // Et on stock les octets reçus

  // Calcul de la température en degré Celsius
  *temp = ((data[1] << 8) | data[0]) * 0.0625;

  // Pas d'erreur
  return true;
}

void Moteino::loop() {
  check_serial();
  check_RF69();
}
