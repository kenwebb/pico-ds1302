/**
 * Ds1302.h
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 * 
 * adapted for Pico by Ken Webb, 27 Oct 2022
 * 
 * 31 Oct 2022
 * I am using additional ideas and code on ds1302 Ram from :
 * https://github.com/msparks/arduino-ds1302  Matt Sparks  no license?
 * "Arduino library for the DS1302 Real Time Clock chip"
 * includes "Setting and accessing the 31 bytes of static RAM. Single-byte and multi-byte (burst) modes are supported."
 *
 */

#ifndef _DS_1302_H
#define _DS_1302_H

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdint.h>

// the following defines are from Arduino.h
// these are needed in Ds1302.cpp
#define HIGH 0x1
#define LOW  0x0
#define INPUT  0x0
#define OUTPUT 0x1

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t dow;
} DateTime;

/**
 * Months of year
 */
enum MONTH {
  MONTH_JAN = 1,
  MONTH_FEB = 2,
  MONTH_MAR = 3,
  MONTH_APR = 4,
  MONTH_MAY = 5,
  MONTH_JUN = 6,
  MONTH_JUL = 7,
  MONTH_AUG = 8,
  MONTH_SET = 9,
  MONTH_OCT = 10,
  MONTH_NOV = 11,
  MONTH_DEC = 12
};

/**
 * Days of week
 */
enum DOW {
  DOW_MON = 1,
  DOW_TUE = 2,
  DOW_WED = 3,
  DOW_THU = 4,
  DOW_FRI = 5,
  DOW_SAT = 6,
  DOW_SUN = 7
};

/**
 * Initializes the DS1302 chip.
 */
void ds1302init(uint8_t pin_rst, uint8_t pin_clk, uint8_t pin_dat);

/**
 * Returns when the oscillator is disabled.
 */
int ds1302isHalted();

/**
 * Stops the oscillator.
 */
void ds1302halt();

/**
 * Returns the current date and time.
 */
void ds1302getDateTime(DateTime* dt);

/**
 * Sets the current date and time.
 */
void ds1302setDateTime(DateTime* dt);

// KSW get/set/test RAM ops
void ds1302getRamByte(uint8_t addr, uint8_t *data);
void ds1302setRamByte(uint8_t addr, uint8_t *data);
void ds1302testRam();

// private:

uint8_t _pin_clk; // CLK GP18
uint8_t _pin_dat; // DAT GP17
uint8_t _pin_rst; // RST GP16 _pin_ena;

void _prepareRead(uint8_t address);
void _prepareWrite(uint8_t address);
void _end();

int _dat_direction;
void _setDirection(int direction);

uint8_t _readByte();
void _writeByte(uint8_t value);
void _nextBit();

uint8_t _dec2bcd(uint8_t dec);
uint8_t _bcd2dec(uint8_t bcd);

// from https://www.arduino.cc/reference/en/
// https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Arduino.h
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
void delayMicroseconds(unsigned int us);

// ksw early test
int ds1302test01(int inum);

// msparks variables from class DS1302

// Size of the DS1302's RAM storage, in bytes
static const int kRamSize = 31;

// msparks functions from class DS1302

/**
 * (msparks) Enables or disables write protection on the chip.
 * While write protection is enabled, all attempts to write to the chip
 * (e.g.,setting the time) will have no effect.
 * The DS1302 datasheet does not define the initial state of write protection,
 * so this method should be called at least once when initializing a
 * device for the first time.
 * @param enable true to enable write protection
 */
void writeProtect(int enable);

/**
 * (msparks) Writes 'len' bytes into RAM from '*data', starting at RAM address 0.
 * @param data: Input data.
 * @param len: Number of bytes of '*data' to read. Must be <= kRamSize.
 */
void writeRamBulk(const uint8_t* data, int len);

/**
 * (msparks) Reads 'len' bytes from RAM into '*data', starting at RAM address 0.
 * @param data: Output data.
 * @param len: Number of bytes of RAM to read. Must be <= kRamSize.
 */
void readRamBulk(uint8_t* data, int len);

#endif // _DS_1302_H

