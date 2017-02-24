#ifndef MOTEINO_SERIALSHELL_H
#define MOTEINO_SERIALSHELL_H

#include<Arduino.h>

class SerialShell;

#include <Moteino.h>

class SerialShell{

public :

void init(Moteino *moteino);

void loop();

private :

#define SERIAL_BUFFER_SIZE 100
// data retrieved from buffer are stored here
char serial_buffer[SERIAL_BUFFER_SIZE+2];

int serial_blength=0;

Moteino *m;

void printParams();

void handleSerialMessage(char *message);

//time in ms at which we stop burning data to rfm69
unsigned long  burn_end=0;

//next time we burn
unsigned long burn_next=0;

};

#endif
