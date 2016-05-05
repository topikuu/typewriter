#!/usr/bin/env python
import argparse
import serial, time

command_buffer = []

charset = ['_', '.', 'w', ')', 'ö', 'k', '^', 'I', 'L', 'ü', '²', 'D', 'F', '?', 'M', '!', 'ä', 'T', 'R', '*', 'K', 'O', 'Ä', 'Ü', '9', '8', '3', '1', '&', '7', 'X', '6', '=', '5', '2', '4', '%', '`', 'Ö', 'Å', '\'', 'μ', '³', '|', 'q', 'å', '°', 'Q', '$', 'Y', 'x', 'W', '/', '£', '§', 'N', 'Z', '0', 'H', '+', 'E', '(', 'U', 'J', 'ß', 'y', 'B', 'P', 'A', 'C', ':', 'S', 'v', 'z', 'p', 'j', 'm', 'V', 'u', 'G', 'l', 'f', 'd', 'o', 'n', '´', 'i', 'g', 'e', 't', 'r', 's', 'a', '"', 'b', 'h', 'c', ';', '-', ',']


def queueCommand(command, data = 0):
    commandDict = {
        "whack": 0x00,
        "lf": 0x01,
        "wheel_cw": 0x02,
        "wheel_ccw": 0x03,
        "corr_up": 0x04,
        "corr_dn": 0x05,
        "slide_l": 0x06,
        "slide_r": 0x07,
        "carr_ret": 0x08
    }
    byte = (((commandDict[command] << 4) & 0xF0) | (data & 0x0F)).to_bytes(1, byteorder="big")
    command_buffer.append(byte)

def whack():
    queueCommand("whack")

def slide(count):
    if count == 0:
        return

    if count > 0:
        command = "slide_r"
    else:
        command = "slide_l"

    count = abs(count)
    while count > 0x0F:
        queueCommand(command, 0x0F)
        count -= 0x0F
    queueCommand(command, count)

def rotateWheel(rotation):
    if rotation == 0:
        return

    if rotation < 0:
        command = "wheel_ccw"
    else:
        command = "wheel_cw"
    
    rotation = abs(rotation)
    while rotation > 0x0F:
        queueCommand(command, 0x0F)
        rotation -= 0x0F
    queueCommand(command, rotation)

def typeChar(c):
    index = charset.index(c)
    rotation = index - typeChar.state
    rotateWheel(rotation)
    whack()
    typeChar.state = index
typeChar.state = 0

def typeSpace():
	slide(1)

def handleChar(c):
    if c in charset:
        typeChar(c)
    elif c == '\n':
        lineFeed()
    elif c == ' ':
        typeSpace()

def parseArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", action = "store", help = "Text file")
    parser.add_argument("--device", action = "store",
                        help = "serial device (COM3, /dev/ttyUSB0)",
                        default = "/dev/ttyUSB0")
    return parser.parse_args()

def main():
    args = parseArgs()
    ser = serial.Serial(args.device, 115200)
    time.sleep(5)
    f = open(args.file)
    while True:
        char = f.read(1)
        if char: # Not EOF
            handleChar(char)
        else:
            break
    f.close()

    while len(command_buffer):
        read_byte = ser.read()
        assert read_byte == 0xAA, "ARDUINO IS BORK'D"
        ser.write(command_buffer.pop())

    ser.close()

if __name__ == '__main__':
    main()
