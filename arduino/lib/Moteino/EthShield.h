#ifndef ARDUINO_LIB_Moteino_EthShield_H_
#define ARDUINO_LIB_Moteino_EthShield_H_

#include <Arduino.h>

#include <Ethernet.h>
#include <utility/w5100.h>

#define ETH_DEBUG

//6 byts to make an ethernet adress
#define ETH_MAC_SIZE 6

class EthShield {
public :

void init();

void loop();

private:

byte ethMac[ETH_MAC_SIZE] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

boolean hasEthernet=false;
byte ETHERNET_PIN=4;
EthernetClient ethc;

};

#endif
