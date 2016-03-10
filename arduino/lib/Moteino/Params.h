#include <Arduino.h>

#ifndef PARAMS_H
#define PARAMS_H

struct NetParams {

	//checksum
	byte chk;

	// set to true once network data has been retrieved
	bool paired;

	// rf69 network number, 0-254 . hardware filtering prevents trame from
	// another network number from reaching this device
  uint8_t rdNet;

	// rf69 crypt key
	char rdKey[RF69_CRYPT_SIZE];
};

struct Params{

    // This is for version detection
    char version[4];

  	byte debug;

  	// rf69 address to use when paired, if not set to RF69_BROADCAST_ADDR
  	uint8_t rdIP;
};

#endif
