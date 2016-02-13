#include "SimpleStepper.h"

SimpleStepper::SimpleStepper(int dir_pin, int step_pin, unsigned int step_delay_us) :
	m_dir_pin(dir_pin),
	m_step_pin(step_pin),
	m_step_delay_us(step_delay_us)
{};

void SimpleStepper::init() {
	pinMode(m_dir_pin, OUTPUT);
	pinMode(m_step_pin, OUTPUT);
	digitalWrite(m_dir_pin, LOW);
	digitalWrite(m_step_pin, LOW);
}

void SimpleStepper::step(int steps) {
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
		delayMicroseconds(m_step_delay_us);
	}
}
