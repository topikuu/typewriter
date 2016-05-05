#include "libraries/SimpleStepper/SimpleStepper.h"
#include "typewriter_fwd.h"

#include <LiquidCrystal.h>

#include <stdint.h>
#include <stdlib.h>

#define DEBUG
#ifdef DEBUG
    #define debugWrite(a, b) digitalWrite(a, b)
#else
    #define debugWrite(a, b)
#endif

// Pin Declarations ------------------------------------------------------------
const int DEBUG_LED_PIN = 13;

const int LCD_RS_PIN = 5;
const int LCD_EN_PIN = 4;
const int LCD_D4_PIN = 12;
const int LCD_D5_PIN = 11;
const int LCD_D6_PIN = 10;
const int LCD_D7_PIN = 9;

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
#ifdef DEBUG
    const unsigned long DEBUG_DISPLAY_DELAY_MS = 1000;
#endif

// Global stuff ----------------------------------------------------------------
SimpleStepper wheel(WHEEL_DIR_PIN, WHEEL_STEP_PIN, 5000);
SimpleStepper slide(SLIDE_DIR_PIN, SLIDE_STEP_PIN, 2000);
#ifdef DEBUG
    LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
#endif

// Serial control --------------------------------------------------------------
typedef void(*OpNoData)();
typedef void(*OpWithData)(uint8_t);

struct SerialCommand {
    uint8_t opcode;
    uint8_t data;
};

struct CommandData {
    const char* name;
    void* address;
    bool has_data_arg;
};

CommandData command_data[] = {
    { "WHACK",      (void*)&whack,                  false },    // 0x00: Whack
    { "LF",         (void*)&linefeed,               false },    // 0x01: Linefeed
    { "WHEEL CW",   (void*)&wheelClockwise,         true },     // 0x02: Wheel clockwise
    { "WHEEL CCW",  (void*)&wheelCounterclockwise,  true },     // 0x03: Wheel counterclockwise
    { "CORR UP",    (void*)&notImplemented,         false },    // 0x04: Correction up
    { "CORR DN",    (void*)&notImplemented,         false },    // 0x05: Correction down
    { "SLIDE L",    (void*)&slideLeft,              true },     // 0x06: Slide left
    { "SLIDE R",    (void*)&slideRight,             true },     // 0x07: Slide right
    { "CARR RET",   (void*)&notImplemented,         false },    // 0x08: Carriage return
};

SerialCommand parseCommand(uint8_t input) {
    uint8_t opcode = (0xF0 & input) >> 4;
    uint8_t data = (0x0F & input);
    #ifdef DEBUG
        lcd.setCursor(0, 0);
        lcd.print(input);
        lcd.print(" ");
        lcd.print(opcode);
        lcd.print(" ");
        lcd.print(data);
        lcd.print("    ");
        delay(5000);
    #endif
    return {opcode, data};
}

void execute(SerialCommand cmd) {
    CommandData data = command_data[cmd.opcode];

    /*
    #ifdef DEBUG
        lcd_debug_command(cmd, data);
    #endif
    */
    
    if (data.has_data_arg) {
        OpWithData op = (OpWithData)data.address;
        op(cmd.data);
    } else {
        OpNoData op = (OpNoData)data.address;
        op();
    }
}

// This is where the magic happens ---------------------------------------------
void whack() {
    debugWrite(DEBUG_LED_PIN, HIGH);
    delayMicroseconds(10*WHACK_TIME_US);
    digitalWrite(WHACK_PIN, HIGH);
    delayMicroseconds(WHACK_TIME_US);
    digitalWrite(WHACK_PIN, LOW);
    delayMicroseconds(10*WHACK_TIME_US);
    debugWrite(DEBUG_LED_PIN, LOW);
}

void linestep() {
    digitalWrite(LF_MOTOR_PIN, HIGH);
    // Wait until motor starts running
    while (!digitalRead(LF_SENSE_PIN)) {
        delayMicroseconds(LF_SENSE_DELAY_US);
    }
    // Wait for sense switch
    while (digitalRead(LF_SENSE_PIN)) {
        delayMicroseconds(LF_SENSE_DELAY_US);
    }
    digitalWrite(LF_MOTOR_PIN, LOW);
    delay(5); // FIXME: y do dis?
}

void linefeed() {
    debugWrite(DEBUG_LED_PIN, HIGH);
    linestep();
    linestep();
    debugWrite(DEBUG_LED_PIN, LOW);
}

void wheelClockwise(uint8_t num_steps) {
    debugWrite(DEBUG_LED_PIN, HIGH);
    wheel.step(int(num_steps)); // TODO: check direction
    debugWrite(DEBUG_LED_PIN, LOW);
}

void wheelCounterclockwise(uint8_t num_steps) {
    debugWrite(DEBUG_LED_PIN, HIGH);
    wheel.step(-int(num_steps)); // TODO: check direction
    debugWrite(DEBUG_LED_PIN, LOW);
}

void slideRight(uint8_t num_steps) {
    debugWrite(DEBUG_LED_PIN, HIGH);
    slide.step(int(num_steps)); // TODO: check direction
    debugWrite(DEBUG_LED_PIN, LOW);
}

void slideLeft(uint8_t num_steps) {
    debugWrite(DEBUG_LED_PIN, HIGH);
    slide.step(-int(num_steps)); // TODO: check direction
    debugWrite(DEBUG_LED_PIN, LOW);
}

void notImplemented() {
    // \:D/
}

#ifdef DEBUG
void lcd_debug_command(SerialCommand& cmd, CommandData& data) {
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(data.name);
    if (data.has_data_arg) {
        lcd.print(" ");
        lcd.print(cmd.data);
    }
    lcd.print("          ");
}

void lcd_debug(const char* command, int data) {
  
}
#endif

// One of these should be called from loop() -----------------------------------
void serialControl() {
    #ifdef DEBUG
        static unsigned long last_cmd = 0;
        static const char* idle[] = { ".", "o", "O", "o" };
    #endif
    static bool requested_flag = false;
    if (Serial.available() == 0 && !requested_flag)
    {
        Serial.write(0xAA);
        requested_flag = true;
    }
    if (Serial.available() > 0)
    {
        uint8_t input = Serial.read();

        execute(parseCommand(input));
        #ifdef DEBUG
            last_cmd = millis();
        #endif
        requested_flag = false;
    }
    #ifdef DEBUG
        else if (millis() > last_cmd + DEBUG_DISPLAY_DELAY_MS) {
            lcd.setCursor(0, 0);
            lcd.print(idle[(millis()/150) % 4]);
            lcd.print("               ");
        }
    #endif
}

void testall() {
    slideLeft(100);
    wheelClockwise(20);
    whack();

    delay(250);
    
    slideRight(100);
    wheelCounterclockwise(20);
    whack();

    delay(250);
/*
    slideLeft(255);
    delay(200);
    slideRight(255);
    delay(200);

    debugWrite(DEBUG_LED_PIN, HIGH);
    wheel.step(-50);
    delay(200);

    debugWrite(DEBUG_LED_PIN, LOW);
    wheel.step(50);
    delay(200);
 */
}

// Arduino functions -----------------------------------------------------------
void setup() {
    pinMode(DEBUG_LED_PIN, OUTPUT);

    pinMode(SLIDE_DIR_PIN, OUTPUT);
    pinMode(SLIDE_STEP_PIN, OUTPUT);
    pinMode(SLIDE_SENSE_PIN, OUTPUT);
    pinMode(WHEEL_DIR_PIN, OUTPUT);
    pinMode(WHEEL_STEP_PIN, OUTPUT);
    pinMode(ERASE_PIN, OUTPUT);
    pinMode(WHACK_PIN, OUTPUT);
    pinMode(LF_MOTOR_PIN, OUTPUT);
    pinMode(LF_SENSE_PIN, OUTPUT);

    // Are these redundant?
    digitalWrite(DEBUG_LED_PIN, LOW);
    digitalWrite(ERASE_PIN, LOW);
    digitalWrite(WHACK_PIN, LOW);
    digitalWrite(LF_MOTOR_PIN, LOW);

    // Steppers
    wheel.init();
    slide.init();

    #ifdef DEBUG
        lcd.begin(16, 2);
    #endif
    Serial.begin(115200);
}

void loop() {
    serialControl();
    //testall();
}
