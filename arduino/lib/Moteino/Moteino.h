/*
 * Moteino.h
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#ifndef ARDUINO_LIB_Moteino_Moteino_H_
#define ARDUINO_LIB_Moteino_Moteino_H_

#include <Arduino.h>
#include <EEPROM.h>
#include <inttypes.h>
#include <SPI.h>

class Moteino;

#include <Params.h>

#include <RF69Manager.h>

using rfm69::RF69Manager;

// version of the code is
// abc where a, b, c in {0, 1..9, a..y, z}
#define MOTEINO_VERSION "006"

class Moteino {

public:

  void setup();
  // call children setup() or initialize() or whatsoever

  // return true if we debug at given level
  boolean debug(int level);

  //////////////////////////////////////////////////////////////////
  // serial port
  /////////////////////////////////////////////////////////////////

  // serial speed
  int SERIAL_BAUD = 9600;

  //////////////////////////////////////////////////////////
  // storing in EEPROM
  //////////////////////////////////////////////////////////

  // store network data that are kept between firmware updates.
  NetParams netparams;

  // structure of internal parameters we store in the EEPROM
  Params params;

  // load params from EEPROM. return false if version mismatch
  boolean loadEEPROM();
  // write present config in EEPROM
  void writeEEPROM();
  // start the EEPROM management (load data, store data if needed)

private:
  void init_EEPROM();

  // write params to the serial
  void printParams();

  // check whether the chk field matches the netparams crc
  boolean chkNetEEPROM();

  // set the chk field in the netparams to the correct value
  void chkSetNet();

  //////////////////////////////////////////////////////////

  // public:
  //  boolean getTemperatureDS18B20(float *temp);

private:
  // 1-wire bus ( http://playground.arduino.cc/Learning/OneWire )
  //byte ONEWIRE_PIN = 3;
  //OneWire owire;

  // onewire address of the DS12B20 if exists
  // DS12B30 = temperature probe
  //byte DS18B20_PIN = 0x28;

  //////////////////////////////////////////////////////////
  // RF69 (wireless radio) chip
  //////////////////////////////////////////////////////////

public:
  RF69Manager radio;

  // connect to a random network, with a random crypt key.
  void rdRandom();

  // flash the led for 2 s and send DISCO on network broadcast
  void rdLedDisco();

  // each element on the network must blink the led
  void rdIdLed();

  // show the radio status with the led
  void radioLed();

private:
  const char *RD_LED_DISCO = "DISCO";

  unsigned long radio_next_led = 0;
  unsigned long radio_count_delay = 3000;
  unsigned long radio_ledcount_duration = 1500;

  static char *ftoa(double f, char *a, int precision) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char *ret = a;
    long heiltal = (long)f;
    itoa(heiltal, a, 10);
    long desimal = abs((long)((f - heiltal) * p[precision]));
    if (desimal == 0)
      return ret;
    while (*a != '\0')
      a++;
    *a++ = '.';
    itoa(desimal, a, 10);
    return ret;
  }

  ///////////////////////////////////////////////////////
  // LED
  ///////////////////////////////////////////////////////

private:
#define LED_PIN 9

#define LED_OFF 0
#define LED_BLK 1
#define LED_FLS 2
#define LED_CNT 3

  unsigned long led_nxtchange = 0;
  unsigned long led_swdelay = 0;
  int led_remains = 0;
  int led_state = LED_OFF;

public:
  void ledBlink(unsigned long delay_ms);

  void ledFlash(unsigned long delay_ms);

  void ledCount(int nb, unsigned long delay_ms, boolean prio = false);

  void loopLed();

  /////////////////////////////////////////////////////////

public:
  void loop();
};

#endif /* ARDUINO_LIB_Moteino_Moteino_H_ */
