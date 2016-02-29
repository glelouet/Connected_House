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

#define VERSION 1
#define ADDRNODEID 10
#define NODEIDTOSETUP 002
#define ADDRNETWORKID 20
#define NETWORKIDTOSETUP 100
#define ADDRGATEWAYID 30
#define GATEWAYIDTOSETUP 001

class Moteino {
	public:

Moteino();
~Moteino();
void setup();//call children setup() or initialize() or whatsoever

//serial speed
int SERIAL_BAUD = 9600;

byte LED_PIN = 13;

// pin of the DS12B20 if exists
byte DS18B20_PIN = 0x28;

// wireless radio
RFM69 radio;
uint32_t ID = 2;
uint16_t NODEID = 2;
long NETWORKID = 100;
long GATEWAYID = 001;
uint8_t crypt_key[16] ;

#ifdef __AVR_ATmega1284P__
  byte FLASH_PIN = 23; // FLASH SS on D23
#else
  byte FLASH_PIN = 8; // FLASH SS on D8
#endif
// self flasher
SPIFlash flash;

// 1-wire bus ( http://playground.arduino.cc/Learning/OneWire )
byte ONEWIRE_PIN = 3;
OneWire owire;

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

void blink(byte PIN, int DELAY_MS);

void sendToGateway(char *data);

boolean getTemperatureDS18B20(float *temp);

};

#endif /* ARDUINO_LIB_Moteino_Moteino_H_ */
