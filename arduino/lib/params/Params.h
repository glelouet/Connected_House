#include <Arduino.h>
#include <RFM69.h>

#ifndef PARAMS_H
#define PARAMS_H

// debug levels
enum debugLvl{
	DEBUG_NONE, DEBUG_WARN, DEBUG_INFO, DEBUG_FULL
};


//16 bytes for a crypt key
#define RF69_CRYPT_SIZE 16

struct NetParams {

	//checksum
	byte chk=0;

	// set to true once network data has been retrieved
	bool paired=false;

	// rf69 network number, 0-254 . hardware filtering prevents trame from
	// another network number from reaching this device
  uint8_t rdNet=100;

	// rf69 address to use when paired, if not set to RF69_BROADCAST_ADDR
	uint8_t rdIP=RF69_BROADCAST_ADDR;

	// rf69 crypt key
	char rdKey[RF69_CRYPT_SIZE]={};// set to 0..0
};

struct Params{

    // This is for version detection
    char version[4];

  	byte debug=DEBUG_FULL;

		unsigned long probePeriod=5000;//5s between probe by default
};

#endif
