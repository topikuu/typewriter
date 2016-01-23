int DIR_PIN = 9;
int STEP_PIN = 8;
int ERASE_PIN = 6;
int WHACK_PIN = 7;
int LF_MOTOR_PIN = 5;
int LF_SENSE_PIN = 4;

int whack_time = 100;

// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.
	pinMode(13, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(STEP_PIN, OUTPUT);
	pinMode(ERASE_PIN, OUTPUT);
	pinMode(WHACK_PIN, OUTPUT);
	pinMode(LF_MOTOR_PIN, OUTPUT);
	pinMode(LF_SENSE_PIN, INPUT_PULLUP);
	digitalWrite(13, LOW);
	digitalWrite(DIR_PIN, LOW);
	digitalWrite(STEP_PIN, LOW);
	digitalWrite(ERASE_PIN, LOW);
	digitalWrite(WHACK_PIN, HIGH);
	digitalWrite(LF_MOTOR_PIN, HIGH);
}

void step(int steps, int delay_time = 200) {
	if (steps < 0) {
		digitalWrite(DIR_PIN, LOW);
		steps = -1 * steps;
	}
	else {
		digitalWrite(DIR_PIN, HIGH);
	}
	delayMicroseconds(500);

	for (int i = 0; i < steps; i++) {
		digitalWrite(STEP_PIN, HIGH);
		delayMicroseconds(20);
		digitalWrite(STEP_PIN, LOW);
		delay(delay_time);
	}
}

void whack() {
	digitalWrite(WHACK_PIN, LOW);
	delay(whack_time);
	digitalWrite(WHACK_PIN, HIGH);
}

void linestep() {
	digitalWrite(LF_MOTOR_PIN, LOW);
	digitalWrite(13, LOW);
	// Wait until motor starts running
	while (!digitalRead(LF_SENSE_PIN)) {
		delayMicroseconds(100);
	}
	digitalWrite(13, HIGH);
	// Wait for sense switch
	while (digitalRead(LF_SENSE_PIN)) {
		delayMicroseconds(100);
	}
	digitalWrite(13, LOW);
	digitalWrite(LF_MOTOR_PIN, HIGH);
	delay(5);
}

void linefeed() {
	linestep();
	linestep();
}

// the loop routine runs over and over again forever:
void loop() {
	digitalWrite(13, HIGH);
	step(-5);

	whack();
	digitalWrite(13, LOW);
	step(5);
	whack();
	linefeed();
	/*
	delay(1000);
	digitalWrite(13, HIGH);
	delay(1000);
	digitalWrite(13, LOW);
	*/
}
