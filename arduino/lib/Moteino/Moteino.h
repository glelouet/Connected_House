/*
 * Moteino.h
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#ifndef ARDUINO_LIB_Moteino_Moteino_H_
#define ARDUINO_LIB_Moteino_Moteino_H_

#include <Arduino.h>
#include <RFM69.h> //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>  //get it here: https://www.github.com/lowpowerlab/spiflash
#include <OneWire.h> // Inclusion de la librairie OneWire
#include <EEPROM.h> //#include <avr/wdt.h>
#include <WirelessHEX69.h> //get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69
#include <Ethernet.h>
#include <utility/w5100.h>

#ifndef DEBUG
#define DEBUG 0
#endif

// version of the code is
// abc where a, b, c in {0, 1..9, a..y, z}
#define VERSION "001"

class Moteino {
	public:

Moteino();
~Moteino();
void setup();//call children setup() or initialize() or whatsoever

//serial speed
int SERIAL_BAUD = 9600;

byte LED_PIN = 13;

//////////////////////////////////////////////////////////
// storing in EEPROM
//////////////////////////////////////////////////////////

// starting element in the EEPROM
int EEPROM_offset = 32;

// strutcutre of internal parameters we store in the EEPROM
struct StoreStruct {
  // This is for version detection
  char version[4];
	// set to true once network data has been retrieved
	boolean netStored;
  // RF69 IP and gateway are stored if they are transmitted by gateway
	byte nodeId, gwId;
	// network number, 0-255 . hardware filtering prevents trame from
	// another network number from reaching this device
	long netWord;
	//do we need to encrypt the network ?
	boolean encrypt;
	//crypt key if encrypt is true
	uint8_t crypt_key[16];
	byte ethMac[6];
} params = {
  VERSION,
  // The default values
	false,
	0,0,
	100,
	false,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED },
};

// set to false to prevent data from beig loaded from EEPROM
boolean acquire_from_EEPROM=true;
// load params from EEPROM. return false if version mismatch
boolean loadEEPROM();
// write present config in EEPROM
void writeEEPROM();
// start the EEPROM management (load data, store data if needed)
void init_EEPROM();
// set to true to store data in EEPROM post setup()
boolean store_EEPROM = false;

//////////////////////////////////////////////////////////

// 1-wire bus ( http://playground.arduino.cc/Learning/OneWire )
byte ONEWIRE_PIN = 3;
OneWire owire;

// onewire address of the DS12B20 if exists
// DS12B30 = temperature probe
byte DS18B20_PIN = 0x28;


//////////////////////////////////////////////////////////
// RF69 (wireless radio) chip
//////////////////////////////////////////////////////////

// wireless radio
RFM69 radio;
// unique ID of flash chip
uint32_t flashId = 1;
boolean acquire_RF69_infos=true;
//init radio
void init_RF69();

// send data to the RF69 gateway if exists
void sendRF69(byte targetId, char *data);

// send data to the broadcast on same network WORD
void sendBCRF69(char *data);

//////////////////////////////////////////////////////////


#ifdef __AVR_ATmega1284P__
  byte FLASH_PIN = 23; // FLASH SS on D23
#else
  byte FLASH_PIN = 8; // FLASH SS on D8
#endif
// self flasher
SPIFlash flash;
//init the flash part
void init_flash();

boolean hasEthernet=0;
byte ETHERNET_PIN=4;
EthernetClient ethc;
// initialize the ethernet client
void init_ethernet();

static char *ftoa(double f,char *a, int precision) {
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

void blink(int DELAY_MS);

boolean getTemperatureDS18B20(float *temp);

void loop();

};

#endif /* ARDUINO_LIB_Moteino_Moteino_H_ */
