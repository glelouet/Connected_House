/*
 * MOTEINO.cpp
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#include <Moteino.h>

Moteino::Moteino():
  owire(ONEWIRE_PIN),
	flash(FLASH_PIN, 0xEF30)//0xEF30 for windbond 4mbit flash
{
}

Moteino::~Moteino(){
}

void Moteino::setup(){
  strcpy(params.version, MOTEINO_VERSION);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUD);
  init_EEPROM();

  // this order is important :
  // flash gives us its unique id so we can translate it to a *mac* address for the RF69
  // we then initialize the RF69 using those parameters
  init_flash();
  radio.init(&netparams);
  radio.findNet();
}

boolean Moteino::debug(int lvl) {
  return params.debug>=lvl;
}

// the cheksum is a series of xor on the params : add[o]^add[1]^..
// the xor is stored at index 0, sor P ^ chk(P) should result in 0
boolean Moteino::chkNetEEPROM(){
  byte chk=0;
  for (size_t index = 0 ; index < sizeof(netparams)  ; ++index) {
    chk=chk^EEPROM[index];
  }
  if(chk!=0 && debug(DEBUG_WARN)) {
    Serial.println(F("netparams check error"));
  }
  return chk==0;
}

void Moteino::init_EEPROM(){
  if(!loadEEPROM())
  writeEEPROM();
}

// xor the values of netparams and store the resulting byte in the first field
void Moteino::chkSetNet(){
  netparams.chk=0;
  for(size_t i=1;i<sizeof(netparams);i++) {
    netparams.chk^= *((byte*) &netparams+i);
  }
}

boolean Moteino::loadEEPROM(){
    if(debug(DEBUG_INFO)) Serial.println(F("read EEPROM"));
    if(chkNetEEPROM()){
      for (size_t t=0; t<sizeof(netparams); t++)
        *((char*)&netparams + t) = EEPROM.read(t);
    }
    size_t paramsOffset=sizeof(netparams);
    if (EEPROM.read(paramsOffset + 0) == MOTEINO_VERSION[0]
        && EEPROM.read(paramsOffset + 1) == MOTEINO_VERSION[1]
        && EEPROM.read(paramsOffset + 2) == MOTEINO_VERSION[2]){
      for (unsigned int t=0; t<sizeof(params); t++)
        *((char*)&params + t) = EEPROM.read(sizeof(netparams) + t);
      if(debug(DEBUG_INFO)) Serial.println(F("read EEPROM ok"));
    } else {
      if(debug(DEBUG_WARN)) Serial.println(F("bad EEPROM"));
      return false;
    }
  return true;
}



void Moteino::writeEEPROM() {
  chkSetNet();
  for (unsigned int t=0; t<sizeof(netparams); t++)
    EEPROM.update(t, *((char*)&netparams + t));
  size_t paramsOffset=sizeof(netparams);
  for (unsigned int t=0; t<sizeof(params); t++)
    EEPROM.update(paramsOffset + t, *((char*)&params + t));
  if(debug(DEBUG_INFO))
    Serial.println(F("EEPROM write OK"));
}

void Moteino::init_flash(){
  if (flash.initialize()) {
    uint8_t* uniq_id = flash.readUniqueId();
    flashId=0;
    for (byte i=4;i<8;i++) {
      flashId=flashId<<8|uniq_id[i];
    }
  }
  else if(debug(DEBUG_WARN))
    Serial.println(F("SPI Flash Init FAIL!"));
}

///////////////////////////////////////////////////////////
// RF69
///////////////////////////////////////////////////////////

void Moteino::rdRandom(){
  netparams.paired=true;
  netparams.rdNet=random(255);
  for(int i=0;i<RF69_CRYPT_SIZE;i++)
   netparams.rdKey[i]=random(255);
  writeEEPROM();
  if(debug(DEBUG_INFO)){
    Serial.println(F("randomed network"));
  }
  radio.findNet();
}

void Moteino::rdLedDisco(){
  ledCount(20, 100, true );
}

void Moteino::rdIdLed(){
  radio.sendBC(RD_LED_DISCO);
  rdLedDisco();
}

void Moteino::radioLed(){
  unsigned long time = millis();
  if(time>=radio_next_led){
    if(radio.isPairing()){
      if(led_state==LED_OFF)
        ledBlink(radio_ledcount_duration);
    } else {
      switch(radio.getState()){
        case TRANSMIT :
          ledCount(1, 400);
          break;
        case PAIRING :
          ledCount(1, 2000);
          break;
        case GETIP :
          ledCount(4, 300);
          break;
        case GETNET :
          ledCount(6, 200);
          break;
        case IDLE :
          break;
      }
    }
    radio_next_led=time+radio_count_delay+radio_ledcount_duration;
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

////////////////////////////////////////////////////////////
// LED
////////////////////////////////////////////////////////////

void Moteino::ledBlink(unsigned long delay_ms) {
  digitalWrite(LED_PIN,HIGH);
  led_state=LED_BLK;
  led_nxtchange=millis()+delay_ms;
}

void Moteino::ledFlash(unsigned long delay_ms){
  digitalWrite(LED_PIN,HIGH);
  led_state=LED_FLS;
  led_nxtchange=millis()+delay_ms;
  led_swdelay=delay_ms;
}

void Moteino::ledCount(int nb, unsigned long delay_ms, boolean prio){
  if(nb<1) return;
  if(prio) led_state=LED_OFF;
  switch(led_state) {
    case LED_OFF :
      led_state=LED_CNT;
      led_remains=nb*2;
      led_swdelay = delay_ms/2;
      led_nxtchange=millis();
    break;
  }
}

void Moteino::loopLed(){
  switch(led_state) {
    case LED_BLK:
      if(led_nxtchange<=millis()) {
        digitalWrite(LED_PIN,LOW);
        led_state=LED_OFF;
      }
    break;
    case LED_FLS:
      if(led_nxtchange<=millis()) {
        led_nxtchange+=led_swdelay;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
    break;
    case LED_CNT:
      if(led_nxtchange<=millis()) {
        led_nxtchange+=led_swdelay;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        led_remains--;
        if(led_remains<=0){
          led_state=LED_OFF;
          digitalWrite(LED_PIN,LOW);
        }
      }
    break;
  }
}

////////////////////////////////////////////////////////

void Moteino::loop() {

  if(radio.hasChg()) writeEEPROM();
  radio.loop();
  if(radio.hasRcv()){
    if (strcmp(RD_LED_DISCO, (char *)radio.getData())==0){
      rdLedDisco();
    } else {
      // check if radio received rom to write on the flash, then flash it
      CheckForWirelessHEX(radio.getRadio(), flash, true);
    }
  }
  radioLed();
  loopLed();
  delay(10);//test
}
