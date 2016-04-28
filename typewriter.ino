#include <stdint.h>
#include <stdlib.h>

#include "libraries/GenericCommandDispatcher/GenericCommandDispatcher.h"
#include "libraries/SimpleStepper/SimpleStepper.h"

// Pin Declarations ------------------------------------------------------------
const int DEBUG_LED_PIN = 13;

const int SLIDE_DIR_PIN = 7;
const int SLIDE_STEP_PIN = 8;
const int SLIDE_SENSE_PIN = 3;
const int WHEEL_DIR_PIN = 16;
const int WHEEL_STEP_PIN = 17;
const int ERASE_PIN = 15;
const int WHACK_PIN = 14;
const int LF_MOTOR_PIN = 6;
const int LF_SENSE_PIN = 2;

// Magic numbers ---------------------------------------------------------------
const unsigned int WHACK_TIME_US = 1500;
const unsigned int LF_SENSE_DELAY_US = 100;

// Serial control definitions --------------------------------------------------
struct SerialCommand {
	static constexpr uint8_t WHACK = 0x00;
	static constexpr uint8_t LINEFEED = 0x01;
	static constexpr uint8_t WHEEL_CW = 0x02;
	static constexpr uint8_t WHEEL_CCW = 0x03;
	static constexpr uint8_t CORRECTION_UP = 0x04;
	static constexpr uint8_t CORRECTION_DOWN = 0x05;
	static constexpr uint8_t SLIDE_LEFT = 0x06;
	static constexpr uint8_t SLIDE_RIGHT = 0x07;
};

using CommandDispatcher = GenericCommandDispatcher<uint8_t, uint8_t, 8>;
using Command = CommandDispatcher::Command;

Command parseCommand(uint8_t input) {
	uint8_t opcode = (0xF0 & input) >> 4;
	uint8_t data = (0x0F & input) >> 4;
	return Command(opcode, data);
}

// Global stuff ----------------------------------------------------------------

CommandDispatcher command_dispatcher;

SimpleStepper wheel(WHEEL_DIR_PIN, WHEEL_STEP_PIN, 5000);
SimpleStepper slide(SLIDE_DIR_PIN, SLIDE_STEP_PIN, 2000);

// This is where the magic happens ---------------------------------------------
void whack(uint8_t) {
	delayMicroseconds(10*WHACK_TIME_US);
	digitalWrite(WHACK_PIN, HIGH);
	delayMicroseconds(WHACK_TIME_US);
	digitalWrite(WHACK_PIN, LOW);
	delayMicroseconds(10*WHACK_TIME_US);
}

void linestep() {
	digitalWrite(LF_MOTOR_PIN, HIGH);
	digitalWrite(DEBUG_LED_PIN, LOW);
	// Wait until motor starts running
	while (!digitalRead(LF_SENSE_PIN)) {
		delayMicroseconds(LF_SENSE_DELAY_US);
	}
	digitalWrite(DEBUG_LED_PIN, HIGH);
	// Wait for sense switch
	while (digitalRead(LF_SENSE_PIN)) {
		delayMicroseconds(LF_SENSE_DELAY_US);
	}
	digitalWrite(DEBUG_LED_PIN, LOW);
	digitalWrite(LF_MOTOR_PIN, LOW);
	delay(5); // FIXME: y do dis?
}

void linefeed(uint8_t) {
	linestep();
	linestep();
}

void wheelClockwise(uint8_t num_steps) {
	wheel.step(int(num_steps)); // TODO: check direction
}

void wheelCounterclockwise(uint8_t num_steps) {
	wheel.step(-int(num_steps)); // TODO: check direction
}

void slideRight(uint8_t num_steps) {
	slide.step(int(num_steps)); // TODO: check direction
}

void slideLeft(uint8_t num_steps) {
	slide.step(-int(num_steps)); // TODO: check direction
}

void correctionUp(uint8_t) {
	// TODO: implement
}

void correctionDown(uint8_t) {
	// TODO: implement
}

// One of these should be called from loop() -----------------------------------
void serialControl() {
	if (Serial.available() > 0)
	{
		uint8_t input = Serial.read();

		command_dispatcher.dispatch(parseCommand(input));
	}
}

void testall() {
	slideLeft(100);
	wheelClockwise(20);
	whack(0);

	delay(250);
	
	slideRight(100);
	wheelCounterclockwise(20);
	whack(0);

	delay(250);
/*
	slideLeft(255);
	delay(200);
	slideRight(255);
	delay(200);

	digitalWrite(DEBUG_LED_PIN, HIGH);
	wheel.step(-50);
	delay(200);

	digitalWrite(DEBUG_LED_PIN, LOW);
	wheel.step(50);
	delay(200);
 */
}

// Arduino functions -----------------------------------------------------------
void setup() {
	pinMode(DEBUG_LED_PIN, OUTPUT);
	pinMode(ERASE_PIN, OUTPUT);
	pinMode(WHACK_PIN, OUTPUT);
	pinMode(LF_MOTOR_PIN, OUTPUT);

	pinMode(LF_SENSE_PIN, INPUT_PULLUP);

	digitalWrite(DEBUG_LED_PIN, LOW);
	digitalWrite(ERASE_PIN, LOW);
	digitalWrite(WHACK_PIN, LOW);
	digitalWrite(LF_MOTOR_PIN, LOW);

	Serial.begin(115200);

	// Serial command bindings
	command_dispatcher.registerOp(SerialCommand::WHACK, whack);
	command_dispatcher.registerOp(SerialCommand::LINEFEED, linefeed);
	command_dispatcher.registerOp(SerialCommand::WHEEL_CW, wheelClockwise);
	command_dispatcher.registerOp(SerialCommand::WHEEL_CCW, wheelCounterclockwise);
	command_dispatcher.registerOp(SerialCommand::CORRECTION_UP, correctionUp);
	command_dispatcher.registerOp(SerialCommand::CORRECTION_DOWN, correctionDown);
	command_dispatcher.registerOp(SerialCommand::SLIDE_LEFT, slideLeft);
	command_dispatcher.registerOp(SerialCommand::SLIDE_RIGHT, slideRight);

	// Steppers
	wheel.init();
	slide.init();
}

void loop() {
	//serialControl();
	testall();
}

