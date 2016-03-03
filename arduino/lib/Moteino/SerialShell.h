#ifndef MOTEINO_SERIALSHELL_H
#define MOTEINO_SERIALSHELL_H

#include<Arduino.h>
#include <Moteino.h>

class SerialShell{

public :

SerialShell();
~SerialShell();

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

};

#endif
