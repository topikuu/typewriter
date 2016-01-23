#include <stdint.h>
#include <stdlib.h>

#include "libraries/GenericCommandDispatcher/GenericCommandDispatcher.h"
#include "libraries/SimpleStepper/SimpleStepper.h"

// Pin Declarations ------------------------------------------------------------
const int DEBUG_LED_PIN = 13;

const int SLIDE_DIR_PIN = 11;
const int SLIDE_STEP_PIN = 10;
const int SLIDE_SENSE_PIN = 9;
const int WHEEL_DIR_PIN = 8;
const int WHEEL_STEP_PIN = 7;
const int ERASE_PIN = 6;
const int WHACK_PIN = 5;
const int LF_MOTOR_PIN = 4;
const int LF_SENSE_PIN = 3;

// Magic numbers ---------------------------------------------------------------
const int WHACK_TIME = 50;
const int LF_SENSE_DELAY_US = 100;

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

SimpleStepper wheel(WHEEL_DIR_PIN, WHEEL_STEP_PIN);
SimpleStepper slide(SLIDE_DIR_PIN, SLIDE_STEP_PIN);

// This is where the magic happens ---------------------------------------------
void whack(uint8_t) {
	digitalWrite(WHACK_PIN, LOW);
	delay(WHACK_TIME);
	digitalWrite(WHACK_PIN, HIGH);
}

void linestep() {
	digitalWrite(LF_MOTOR_PIN, LOW);
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
	digitalWrite(LF_MOTOR_PIN, HIGH);
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
	digitalWrite(DEBUG_LED_PIN, HIGH);
	wheel.step(-5, 4);

	whack(0);
	digitalWrite(DEBUG_LED_PIN, LOW);
	wheel.step(5, 4);
	whack(0);
	delay(50);
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
	digitalWrite(WHACK_PIN, HIGH);
	digitalWrite(LF_MOTOR_PIN, HIGH);

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
	serialControl();
	//testall();
}

