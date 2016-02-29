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
void setup();
//call children setup() or initialize() or whatsoever

//////////////////////////////////////////////////////////////////
// serial port
/////////////////////////////////////////////////////////////////

//serial speed
int SERIAL_BAUD = 9600;

#define SERIAL_BUFFER_SIZE 100
// data retrieved from buffer are stored here
char serial_buffer[SERIAL_BUFFER_SIZE+2];
int serial_blength=0;
//retrieve orders from serial
void check_serial();
void handleSerialMessage(char *message);

byte LED_PIN = 9;

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
	byte netWord;
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


//////////////////////////////////////////////////////////
// RF69 (wireless radio) chip
//////////////////////////////////////////////////////////

//default gateway address
byte gw_RF69=0;
// wireless radio
RFM69_ATC radio;
// unique ID of flash chip
uint32_t flashId = 1;
boolean acquire_RF69_infos=true;
//init radio
void init_RF69();

// send data to a RF69 station if exists
void sendRF69(char *data, byte targetId);

// send data to the broadcast on same network WORD
void sendBCRF69(char *data);

// set pairing mode on, answering netword=gw_addr to broadcast on net word
void pairOn();

//deactivate pairing mod
void pairOff();

//check if data istransmitted through RF69
void check_RF69();

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
