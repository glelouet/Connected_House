/*
 * MONTEINO.cpp
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#include "MONTEINO.h"

MONTEINO::MONTEINO() {
	// TODO Auto-generated constructor stub

}

MONTEINO::~MONTEINO() {
	// TODO Auto-generated destructor stub
}



//turn the LED on
void MONTEINO::on(){
  digitalWrite(ledPin,HIGH); //set the pin HIGH and thus turn LED on
}

//turn the LED off
void MONTEINO::off(){
  digitalWrite(ledPin,LOW); //set the pin LOW and thus turn LED off
}

//make the LED blink
void MONTEINO::blink(int time){
  on();                                             // sets the LED on
  delay(time/2);                // waits for a second
  off();                                            // sets the LED off
  delay(time/2);                // waits for a second
}

