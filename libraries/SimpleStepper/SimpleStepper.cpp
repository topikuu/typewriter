#include "SimpleStepper.h"

SimpleStepper::SimpleStepper(int dir_pin, int step_pin) :
	m_dir_pin(dir_pin),
	m_step_pin(step_pin)
{};

void SimpleStepper::init() {
	pinMode(m_dir_pin, OUTPUT);
	pinMode(m_step_pin, OUTPUT);
	digitalWrite(m_dir_pin, LOW);
	digitalWrite(m_step_pin, LOW);
}

void SimpleStepper::step(int steps, int delay_time) {
	if (steps < 0) {
		digitalWrite(m_dir_pin, LOW);
		steps = -1 * steps;
	}
	else {
		digitalWrite(m_dir_pin, HIGH);
	}
	delayMicroseconds(DIR_DELAY_US);

	for (int i = 0; i < steps; i++) {
		digitalWrite(m_step_pin, HIGH);
		delayMicroseconds(PULSE_DELAY_US);
		digitalWrite(m_step_pin, LOW);
		delay(delay_time);
	}
}
