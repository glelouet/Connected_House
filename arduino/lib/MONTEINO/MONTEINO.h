/*
 * MONTEINO.h
 *
 *  Created on: 24 févr. 2016
 *      Author: guillaume
 */

#ifndef ARDUINO_LIB_MONTEINO_MONTEINO_H_
#define ARDUINO_LIB_MONTEINO_MONTEINO_H_

#include <arduino>

class MONTEINO {
public:
	MONTEINO();
	virtual ~MONTEINO();
    void on();
    void off();
    void blink(int time);
};
#endif /* ARDUINO_LIB_MONTEINO_MONTEINO_H_ */

