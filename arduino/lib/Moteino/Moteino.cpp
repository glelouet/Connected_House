/*
 * MOTEINO.cpp
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#include <Moteino.h>
#include <Arduino.h>

void Moteino::blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

Moteino::Moteino():
	flash(FLASH_PIN, 0xEF30),//0xEF30 for windbond 4mbit flash
	owire(ONEWIRE_PIN){
}

Moteino::~Moteino(){
}

void Moteino::setup(){
 Serial.begin(SERIAL_BAUD);
  // Setup of the ID
  #ifdef SETUPEEPROM
  	Serial.println("setting EEPROM");
    EEPROM.write(ADDRNODEID, NODEIDTOSETUP);
    EEPROM.write(ADDRNETWORKID, NETWORKIDTOSETUP);
    EEPROM.write(ADDRGATEWAYID, GATEWAYIDTOSETUP);
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
    GATEWAYID = EEPROM.read(ADDRGATEWAYID);
  #else
  	Serial.println("loading EEPROM");
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
    GATEWAYID = EEPROM.read(ADDRGATEWAYID);
  #endif
  pinMode(LED_PIN, OUTPUT);

  if (flash.initialize()) {
  	uint8_t* uniq_id = flash.readUniqueId();
//  	Serial.print("flah id : ");
//  	for (byte i=0;i<8;i++)
//    {
//      Serial.print(uniq_id[i], HEX);
//      Serial.print(' ');
//    }
    ID=0;
  	for (byte i=4;i<8;i++) {
    	ID=ID<<8|uniq_id[i];
    }
  	Serial.print("device id=");
  	Serial.println(ID);
//    Serial.println("SPI Flash Init OK.");
  }
  else
    Serial.println("SPI Flash Init FAIL!");

  radio.initialize(RF69_433MHZ,NODEID,NETWORKID);
  //radio.encrypt(ENCRYPTKEY); //OPTIONAL
  #ifdef IS_RFM69HW //only for RFM69HW
    radio.setHighPower();!
  #endif

}


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

void Moteino::sendToGateway(char *buff){
  char trame[100];
  memset(trame,'\0',100);
  sprintf(trame,"%ld;%ld;%u;%lu;%u;",NETWORKID,GATEWAYID,NODEID,ID,VERSION);
  strcat(trame, buff);
  int sendSize = strlen(trame);

  // Send a message to rf69_server
  uint8_t data[100];
  for (int i=0;i<100;i++)
    data[i]=trame[i];
  blink(LED,3);

  radio.sendWithRetry(GATEWAYID, data, sendSize);
}
