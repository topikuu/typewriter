#pragma once

void whack();
void linefeed();
void wheelClockwise(uint8_t num_steps);
void wheelCounterclockwise(uint8_t num_steps);
void slideRight(uint8_t num_steps);
void slideLeft(uint8_t num_steps);
void notImplemented();

struct SerialCommand;
struct CommandData;

void lcd_debug_command(SerialCommand& cmd, CommandData& data);
void lcd_debug(const char*, int = -1);
