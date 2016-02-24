/*
 * MOTEINO.cpp
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#include <Moteino.h>

#include <Arduino.h>


char *ftoa(double f, char *a,int precision)
{
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

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}


