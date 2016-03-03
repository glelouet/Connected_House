/*
 * Moteino.h
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#ifndef ARDUINO_LIB_Moteino_Moteino_H_
#define ARDUINO_LIB_Moteino_Moteino_H_

#include <Arduino.h>
#include <RFM69_ATC.h> //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>  //get it here: https://www.github.com/lowpowerlab/spiflash
#include <OneWire.h> // Inclusion de la librairie OneWire
#include <EEPROM.h> //#include <avr/wdt.h>
#include <WirelessHEX69.h> //get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69
#include <Ethernet.h>
#include <utility/w5100.h>

// version of the code is
// abc where a, b, c in {0, 1..9, a..y, z}
#define VERSION "004"

//16 bytes for a crypt key
#define RF69_CRYPT_SIZE 16

//6 byts to make an ethernet adress
#define ETH_MAC_SIZE 6

// debug levels
#define DEBUG_NONE 0
#define DEBUG_WARN 1
#define DEBUG_INFO 2
#define DEBUG_FULL 3

#define INCLUDE_DEBUG

class Moteino {
	public:

Moteino();
~Moteino();
void setup();
//call children setup() or initialize() or whatsoever

//return true if we debug at given level
boolean debug(int level);

//////////////////////////////////////////////////////////////////
// serial port
/////////////////////////////////////////////////////////////////

//serial speed
int SERIAL_BAUD = 9600;

//////////////////////////////////////////////////////////
// storing in EEPROM
//////////////////////////////////////////////////////////

// starting element in the EEPROM
int EEPROM_offset = 32;

// structure of internal parameters we store in the EEPROM
struct StoreStruct {
  // This is for version detection
  char version[4];
	byte debug;
	// set to true once network data has been retrieved
	bool paired;
	// set to true to always start in pairing mode
	bool pairing;
	// rf69 network number, 0-254 . hardware filtering prevents trame from
	// another network number from reaching this device
	uint8_t rdNet;
	// rf69 address to use when paired, if not set to RF69_BROADCAST_ADDR
	uint8_t rdIP;
	// rf69 crypt key
	char rdKey[RF69_CRYPT_SIZE];
	byte ethMac[ETH_MAC_SIZE];
} params = {
  VERSION,
  // The default values
	DEBUG_FULL,
	false,false,
	100,
	RF69_BROADCAST_ADDR,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }
};

// load params from EEPROM. return false if version mismatch
boolean loadEEPROM();
// write present config in EEPROM
void writeEEPROM();
// start the EEPROM management (load data, store data if needed)
void init_EEPROM();
// set to true to store data in EEPROM post setup()
boolean rewrite_EEPROM = false;
// write params to the serial
void printParams();

//////////////////////////////////////////////////////////

// 1-wire bus ( http://playground.arduino.cc/Learning/OneWire )
byte ONEWIRE_PIN = 3;
OneWire owire;

// onewire address of the DS12B20 if exists
// DS12B30 = temperature probe
byte DS18B20_PIN = 0x28;

boolean getTemperatureDS18B20(float *temp);


//////////////////////////////////////////////////////////
// RF69 (wireless radio) chip
//////////////////////////////////////////////////////////

public :

//select a random network, a random crypt key, and connect itself.
void rdRandom();

// return the present connection state of the radio : idle, search netowrk, set ip, transmit
int rdState();

// send data to a RF69 station if exists and connected
void sendRF69(char *data, byte targetId);

// send data to the broadcast on same network WORD
void sendBCRF69(char *data);

// search radio network and crypt key
void rdSearchNet();

//set radio network
void rdSetNet(uint8_t net);

// find the net to use
void rdGetNet();

// search radio ip
void rdSearchIP();

//set the ip to use
void rdSetIP(uint8_t ip);

// find the IP to use
void rdGetIP();

bool rdPairing();

// set pairing mode on, answering rdNet:key to broadcast on net word
void rdPairOn();

//deactivate pairing mod
void rdPairOff();

//flash the led for 2 s and send DISCO on network broadcast
void rdLedDisco();

//each element on the network must blink the led
void rdIdLed();

private :

//default gateway address
byte gw_RF69=0;
// wireless radio
RFM69_ATC radio;

#define RADIO_IDLE 0
#define RADIO_GETNET 1
#define RADIO_GETIP 2
#define RADIO_TRANSMIT 3
#define RADIO_PAIRING 4
int radio_state=RADIO_IDLE;

#define RADIO_SCANNET 255

unsigned long last_scan=0;
unsigned long scan_net_delay=2000;
//init radio
void init_RF69();

#define RF69_PAIRING_MS 60000 //1min of pairing time

//time at which we deactivate pairing
unsigned long pairingEnd=0;

char *RD_NET_DISCO="REQ"VERSION;
char *RD_LED_DISCO="DISCO";
char *RD_IP_DISCO="ping";

// periodically send network request.
void rdLoopScanNet();

//last IP we sent message to
uint8_t radio_scan_ip=0;
bool radio_scan_ip_answered=false;
unsigned long radio_last_ip_request;
unsigned long radio_iprequest_delay=300; //300ms between each IP request

// find first IP not in use.
void rdLoopScanIP();

unsigned long radio_next_led=0;
unsigned long radio_count_delay=3000;
unsigned long radio_ledcount_duration=1000;

void rdLoopPairing();

void rdLoopTransmit();

public :

//check if data istransmitted through RF69
void check_RF69();

//show the radio status with the led
void radioLed();

//////////////////////////////////////////////////////////

public :

#ifdef __AVR_ATmega1284P__
  byte FLASH_PIN = 23; // FLASH SS on D23
#else
  byte FLASH_PIN = 8; // FLASH SS on D8
#endif
// self flasher
SPIFlash flash;
// unique ID of flash chip
uint32_t flashId = 1;
//init the flash part
void init_flash();

boolean hasEthernet=false;
byte ETHERNET_PIN=4;
EthernetClient ethc;
// initialize the ethernet client
void init_ethernet();

static char *ftoa(double f,char *a, int precision) {
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 long desimal = abs((long)((f - heiltal) * p[precision]));
 if(desimal==0) return ret;
 while (*a != '\0') a++;
 *a++ = '.';
 itoa(desimal, a, 10);
 return ret;
}

///////////////////////////////////////////////////////
// LED
///////////////////////////////////////////////////////

private :

#define LED_PIN  9

#define LED_OFF 0
#define LED_BLK 1
#define LED_FLS 2
#define LED_CNT 3

unsigned long led_nxtchange=0;
unsigned long led_swdelay=0;
int led_remains=0;
int led_state=LED_OFF;

public :

void ledBlink(unsigned long delay_ms);

void ledFlash(unsigned long delay_ms);

void ledCount(int nb, unsigned long delay_ms, boolean prio=false);

void check_led();

/////////////////////////////////////////////////////////

public :

void loop();

};

#endif /* ARDUINO_LIB_Moteino_Moteino_H_ */
