/*
 * Moteino.h
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#ifndef ARDUINO_LIB_Moteino_Moteino_H_
#define ARDUINO_LIB_Moteino_Moteino_H_

#include <Arduino.h>
#include <SPIFlash.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <Params.h>

//request here to be able to load the libs in other classes
#include <RFM69_ATC.h>
#include <Ethernet.h>
#include <WirelessHEX69.h>


// version of the code is
// abc where a, b, c in {0, 1..9, a..y, z}
#define MOTEINO_VERSION "005"

//16 bytes for a crypt key
#define RF69_CRYPT_SIZE 16

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

//store network data that are kept between firmware updates.
NetParams netparams= {
	0,
	false,
	100,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,MOTEINO_VERSION[0],MOTEINO_VERSION[1],MOTEINO_VERSION[2]}
};

// structure of internal parameters we store in the EEPROM
Params params = {
  MOTEINO_VERSION,
  // The default values
	DEBUG_FULL,
	RF69_BROADCAST_ADDR
};

// load params from EEPROM. return false if version mismatch
boolean loadEEPROM();
// write present config in EEPROM
void writeEEPROM();
// start the EEPROM management (load data, store data if needed)

private :

size_t paramsOffset = sizeof(netparams);

void init_EEPROM();
// set to true to store data in EEPROM post setup()
boolean rewrite_EEPROM = false;
// write params to the serial
void printParams();

//check whether the chk field matches the netparams crc
boolean chkNetEEPROM();

//set the chk field in the netparams to the correct value
void chkSetNet();

//////////////////////////////////////////////////////////

public :

boolean getTemperatureDS18B20(float *temp);

private :

// 1-wire bus ( http://playground.arduino.cc/Learning/OneWire )
byte ONEWIRE_PIN = 3;
OneWire owire;

// onewire address of the DS12B20 if exists
// DS12B30 = temperature probe
byte DS18B20_PIN = 0x28;


//////////////////////////////////////////////////////////
// RF69 (wireless radio) chip
//////////////////////////////////////////////////////////


public :

	RF69Manager getRadio();

	//connect to a random network, with a random crypt key.
	void rdRandom();

	// find the network to use
	void rdFindNet();

	// find the IP to use
	void rdFindIP();

	//flash the led for 2 s and send DISCO on network broadcast
	void rdLedDisco();

	//each element on the network must blink the led
	void rdIdLed();

	//show the radio status with the led
	void radioLed();


private :

	RF69Manager radio;


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
