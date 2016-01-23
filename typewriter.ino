#include <stdint.h>

const int SLIDE_DIR_PIN = 11;
const int SLIDE_STEP_PIN = 10;
const int WHEEL_DIR_PIN = 9;
const int WHEEL_STEP_PIN = 8;
const int ERASE_PIN = 6;
const int WHACK_PIN = 7;
const int LF_MOTOR_PIN = 5;
const int LF_SENSE_PIN = 4;

const int WHACK_TIME = 100;

class Stepper {
public:
  Stepper(int dir_pin, int step_pin) :
    m_dir_pin(dir_pin),
    m_step_pin(step_pin)
  {};

  void init() {
    pinMode(m_dir_pin, OUTPUT);
    pinMode(m_step_pin, OUTPUT);
    digitalWrite(m_dir_pin, LOW);
    digitalWrite(m_step_pin, LOW);
  }
  
  void step(int steps, int delay_time = 200) {
    if (steps < 0) {
      digitalWrite(m_dir_pin, LOW);
      steps = -1 * steps;
    }
    else {
      digitalWrite(m_dir_pin, HIGH);
    }
    delayMicroseconds(500);
  
    for (int i = 0; i < steps; i++) {
      digitalWrite(m_step_pin, HIGH);
      delayMicroseconds(20);
      digitalWrite(m_step_pin, LOW);
      delay(delay_time);
    }
  }
private:
  int m_dir_pin;
  int m_step_pin;
};

Stepper wheel(WHEEL_DIR_PIN, WHEEL_STEP_PIN);
Stepper slide(SLIDE_DIR_PIN, SLIDE_STEP_PIN);

// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.
	pinMode(13, OUTPUT);
	pinMode(ERASE_PIN, OUTPUT);
	pinMode(WHACK_PIN, OUTPUT);
	pinMode(LF_MOTOR_PIN, OUTPUT);
	pinMode(LF_SENSE_PIN, INPUT_PULLUP);
	digitalWrite(13, LOW);
	digitalWrite(ERASE_PIN, LOW);
	digitalWrite(WHACK_PIN, HIGH);
	digitalWrite(LF_MOTOR_PIN, HIGH);

	Serial.begin(115200);
}

enum class SerialCommand{
	WHACK = 0x00,
	LINEFEED = 0x01,
	WHEEL_CW = 0x02,
	WHEEL_CCW = 0x03,
	CORRECTION_UP = 0x04,
	CORRECTION_DOWN = 0x05,
	CHAR_LEFT = 0x06,
	CHAR_RIGHT = 0x07
};

void whack() {
	digitalWrite(WHACK_PIN, LOW);
	delay(WHACK_TIME);
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

void serialControl() {
	if (Serial.available() > 0)
	{
		// read the incoming byte:
		uint8_t input = Serial.read();

		uint8_t command = (0xF0 & input) >> 4;
		uint8_t data = (0x0F & input) >> 4;

	}
}

// the loop routine runs over and over again forever:
void testall() {
  /*
	digitalWrite(13, HIGH);
	step(-5);

	whack();
	digitalWrite(13, LOW);
	step(5);
	whack();
	linefeed();
	
	delay(1000);
	digitalWrite(13, HIGH);
	delay(1000);
	digitalWrite(13, LOW);
	*/
}

void loop() {
  testall();
}

