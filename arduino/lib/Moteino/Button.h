#ifndef MOTEINO_BTN_H
#define MOTEINO_BTN_H

#include<Arduino.h>

#define BTN_MAX_PUSHES 7
#define BTN_PUSH_GRAN 100

/*
 A Buton captures a series of pushes.
 A series is ended by a >=1s long "off", meaning no push for at least 1s.
 each byte of the series contains the duration of the push, base 100ms.
  	A push can thus last from 100ms to 25.5 s
 The series contains at max BTN_MAX_PUSHES pushes durations, and 0 is aded at the end.
 if too many pushes are added the first 5 registered pushes are discarded.
*/
class Button {

public :

	void init(uint8_t btn_pin, unsigned long idle_delay);

	void check();

	bool hasSeries();

	// get the last series stored, also set hasSeries to false until a new series is stored
	byte* pop();

	int anaRead();

private :

	uint8_t m_btn_pin;
	unsigned long m_idle_delay;

	// last registered btn value
	boolean last_val=false;

	//set to true when no call to getLastSeries since last series was retrieved
	boolean m_has_series=false;

	//set to true when we are writing a series
	boolean write=false;

	// last time the btn was switched from 0 to 1
	unsigned long last_switch_time;

	//number of pushes written for present series, if write is true
	int series_nb_vals=0;

	byte frt_buffer[BTN_MAX_PUSHES+1];
	byte bck_buffer[BTN_MAX_PUSHES+1];
	// when true write series in bckbuffer, when false write in frtbuffer
	boolean writeBackBuffer=false;

};

#endif
