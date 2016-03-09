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
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUD);
  init_EEPROM();

  // this order is important :
  // flash gives us its unique id so we can translate it to a *mac* address for the RF69
  // we then initialize the RF69 using those parameters
  init_flash();
  init_RF69();
  if(rewrite_EEPROM) writeEEPROM();
}

boolean Moteino::debug(int lvl) {
  return params.debug>=lvl;
}

// the cheksum is a series of xor on the params : add[o]^add[1]^..
// the xor is stored at index 0, sor P ^ chk(P) should result in 0
boolean Moteino::chkNetEEPROM(){
  byte chk=0;
  for (int index = 0 ; index < sizeof(netparams)  ; ++index) {
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
  for(int i=1;i<sizeof(netparams);i++) {
    netparams.chk^= *((byte*) &netparams+i);
  }
}

boolean Moteino::loadEEPROM(){
    if(debug(DEBUG_INFO)) Serial.println(F("read EEPROM"));
    if(chkNetEEPROM()){
      for (size_t t=0; t<sizeof(netparams); t++)
        *((char*)&netparams + t) = EEPROM.read(t);
    }
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

void Moteino::init_RF69(){
  //init with any net/IP then ask to get them
  radio.initialize(RF69_433MHZ,0,0);
  rdFindNet();
  #ifdef IS_RFM69HW //only for RFM69HW
    radio.setHighPower();
  #endif
  radio.enableAutoPower(-60);
}

void Moteino::rdRandom(){
  netparams.paired=true;
  netparams.rdNet=random(255);
  for(int i=0;i<RF69_CRYPT_SIZE;i++)
   netparams.rdKey[i]=random(255);
  writeEEPROM();
  if(debug(DEBUG_INFO)){
    Serial.println(F("randomed network"));
  }
  rdFindNet();
}

int Moteino::rdState() {
  return radio_state;
}

void Moteino::rdSearchNet(){
  netparams.paired=false;
  writeEEPROM();
  rdFindNet();
}

void Moteino::rdSetNet(uint8_t net){
  netparams.rdNet=net;
  netparams.paired=true;
  radio.setNetwork(net);
  writeEEPROM();
  rdFindIP();
}

void Moteino::rdFindNet(){
  if(netparams.paired) {
    radio.setNetwork(netparams.rdNet);
    radio.encrypt(netparams.rdKey);
    rdFindIP();
  } else {
    radio_state=RADIO_GETNET;
    last_scan=0;
    radio.promiscuous();
    radio.setNetwork(RADIO_SCANNET);
    radio.encrypt(0);
  }
}

void Moteino::rdLoopScanNet(){
  if (radio.receiveDone()
      && radio.DATALEN==3+1+RF69_CRYPT_SIZE
      && strncmp((char *)radio.DATA, "net", 3)==0 ) {
    netparams.paired=true;
    netparams.rdNet=radio.DATA[3];
    for(int i=0;i<RF69_CRYPT_SIZE;i++) {
      netparams.rdKey[i]=radio.DATA[i+4];
    }
    radio.promiscuous(false);
    writeEEPROM();
    rdLedDisco();
    rdFindNet();
  } else if(millis()-last_scan>scan_net_delay) {
    last_scan=millis();
    sendBCRF69(RD_NET_DISCO);
  }
}

void Moteino::rdSearchIP(){
  if(radio_state==RADIO_GETIP) return;
  //change the IP in params to broadcast
  if(params.rdIP!=RF69_BROADCAST_ADDR) {
    params.rdIP=RF69_BROADCAST_ADDR;
    writeEEPROM();
  }
  radio_state=RADIO_GETIP;
  radio_ip=RF69_BROADCAST_ADDR;
  radio.setAddress(RF69_BROADCAST_ADDR);
  radio.promiscuous();
  radio.receiveDone();
}

void Moteino::rdSetIP(uint8_t ip){
  params.rdIP=ip;
  radio_ip=ip;
  radio.setAddress(ip);
  radio_state = RADIO_TRANSMIT;
  writeEEPROM();
}

void Moteino::rdFindIP() {
    if(params.rdIP!=RF69_BROADCAST_ADDR) {
      radio_ip=params.rdIP;
      radio.setAddress(radio_ip);
      radio_state = RADIO_TRANSMIT;
    } else {
      rdSearchIP();
    }
}

uint8_t Moteino::rdIp(){
  return radio_ip;
}

void Moteino::rdLoopScanIP(){
  ++radio_ip;
  if(radio_ip==gw_RF69) radio_ip++;
  bool ack = rdSendSync(RD_IP_DISCO, radio_ip);
  if(!ack) {
    //maybe we can get this IP. We may also be competing for this IP : wait random
    delay(random(1000));
    if(rdSendSync(RD_IP_DISCO, radio_ip)) return;
    radio.promiscuous(false);
    radio.setAddress(radio_ip);
    radio_state = RADIO_TRANSMIT;
    if(debug(DEBUG_INFO)) {
      Serial.print(F("acquired IP "));
      Serial.println(radio_ip);
    }
  }
}

void Moteino::rdPairOn(){
  radio_state=RADIO_PAIRING;
  pairingEnd=millis()+RF69_PAIRING_MS;
  radio.promiscuous();
  radio.setNetwork(RADIO_SCANNET);
  radio.encrypt(0);
}

void Moteino::rdPairOff(){
  radio.promiscuous(false);
  rdFindNet();
}

bool Moteino::rdPairing(){
  return radio_state==RADIO_PAIRING;
}

void Moteino::rdLoopPairing(){
  if(pairingEnd<=millis()) {
    if(debug(DEBUG_INFO))
      Serial.println(F("pairing mode timeout"));
    rdPairOff();
  } else {
    if (radio.receiveDone()) {
      if(strcmp(RD_NET_DISCO, (char *)radio.DATA)==0) {
        char data[3+1+RF69_CRYPT_SIZE+1];//"net",netId, cryptkey, 0
        data[0]='n';data[1]='e';data[2]='t';
        data[3]=netparams.rdNet;
        for(int i=0;i<RF69_CRYPT_SIZE;i++){
          data[i+4]=netparams.rdKey[i];
        }
        data[3+1+RF69_CRYPT_SIZE]=0;
        sendBCRF69(data);
        ledCount(10,100,true);
      }
    }
  }
}

void Moteino::rdLoopTransmit(){
  if (radio.receiveDone()) {
    if(radio.SENDERID==radio_ip){
      // another station has same IP : get the IP again
      if(debug(DEBUG_WARN)){
        Serial.print(F("collision on IP"));
        Serial.println(radio_ip);
      }
      rdFindIP();
      return;
    }
    m_rdRcv = true;
    if (strcmp(RD_LED_DISCO, (char *)radio.DATA)==0){
      rdLedDisco();
    } else if (strcmp(RD_IP_DISCO, (char *)radio.DATA)==0){
      radio.sendACK();
    }else {
      // check if radio received rom to write on the flash, then flash it
      CheckForWirelessHEX(radio, flash, true);
    }
  }
}

void Moteino::rdLedDisco(){
  ledCount(20, 100, true );
}

void Moteino::rdIdLed(){
  sendBCRF69(RD_LED_DISCO);
  rdLedDisco();
}

bool Moteino::rdSendSync(char *trame, byte targetId){
  return radio.sendWithRetry(targetId, ((uint8_t *) trame),  strlen(trame));
}

void Moteino::rdSendAsync(char *trame, byte targetId, bool ack){
  radio.send(targetId, ((uint8_t *) trame),  strlen(trame), ack);
}

void Moteino::sendBCRF69(char *data){
  radio.send(RF69_BROADCAST_ADDR, data, strlen(data));
}

void Moteino::check_RF69(){
  m_rdRcv=false;
  switch (radio_state) {
    case RADIO_GETNET :
      rdLoopScanNet();
    break;
    case RADIO_GETIP:
      rdLoopScanIP();
    break;
    case RADIO_TRANSMIT :
      rdLoopTransmit();
    break;
    case RADIO_PAIRING :
      rdLoopPairing();
    break;
  }
  radioLed();
}

bool Moteino::rdRcv(){
  return m_rdRcv;
}


void Moteino::radioLed(){
  unsigned long time = millis();
  if(time>=radio_next_led){
    if(rdPairing()){
      if(led_state==LED_OFF)
        ledBlink(radio_ledcount_duration);
    } else {
      int count = 2*(RADIO_TRANSMIT-radio_state+1);
      int period = 2*radio_ledcount_duration/(count*2);
      ledCount(count, period);
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

void Moteino::check_led(){
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
  check_RF69();
  check_led();
}
