#ifndef SIMPLESTEPPER_HEADER_GUARD_
#define SIMPLESTEPPER_HEADER_GUARD_

#include "Arduino.h"

class SimpleStepper {
public:
	SimpleStepper(int dir_pin, int step_pin, unsigned int step_delay_us = 10000);
	void init();
	void step(int steps);
private:
	int m_dir_pin;
	int m_step_pin;
	unsigned int m_step_delay_us;
	static const int DIR_DELAY_US = 500;
	static const int PULSE_DELAY_US = 20;
};

#endif /* end of include guard: SIMPLESTEPPER_HEADER_GUARD_ */
