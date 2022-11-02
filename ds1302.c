/**
 * ds1302.c
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 * 
 * adapted for Pico by Ken Webb, 27 Oct 2022
 * 
 * 31 Oct 2022
 * I am using additional ideas on ds1302 Ram from :
 * https://github.com/msparks/arduino-ds1302
 * includes "Setting and accessing the 31 bytes of static RAM. Single-byte and multi-byte (burst) modes are supported."
 * 
 * TODO:
 * - msparks has 4 burst commands on p.1 of DS1302.cpp; I should use these ???
 * - 
 */

#include "ds1302.h"

// see ds1302.h

// KSW added names from Dallas Semiconductor spec, as comments
// REGISTER ADDRESS
// A. CLOCK
// Note: bit 0 is RD/W  RD=1 W=0
// Note: bit 6 is 0
// bit order: 7 6 5 4 3 2 1 0
#define REG_SECONDS  0x80 // SEC
#define REG_MINUTES  0x82 // MIN
#define REG_HOUR     0x84 // HR
#define REG_DATE     0x86 // DATE
#define REG_MONTH    0x88 // MONTH
#define REG_DAY      0x8A // DAY
#define REG_YEAR     0x8C // YEAR
#define REG_WP       0x8E // CONTROL  (msparks kWriteProtectReg 7)
// TRICKLE CHARGER 1001000_ 0x90
#define REG_BURST    0xBE // CLOCK BURST

// B: RAM  (from KSW, Dallas Semiconductor spec)
// Note: bit 6 is 1
#define REG_RAM00     0xC0
#define REG_RAM01     0xC2
// ...
#define REG_RAM29     0xFA
#define REG_RAM30     0xFC
#define REG_RAM_BURST 0xFE  //(msparks kRamBurstWrite)
// msparks kRamBurstRead 0xFF)

/**
 * Use same pins as in Pico python version.
 * ds = DS1302(Pin(18), Pin(17), Pin(16));
 * ex: init(16, 18, 17);
 * @param pin_rst  RST - GP16 (PIN21)
 * @param pin_clk  CLK - GP18 (PIN24)
 * @param pin_dat  DAT - GP17 (PIN22)
 */
void ds1302init(uint8_t pin_rst, uint8_t pin_clk, uint8_t pin_dat) {
  _pin_rst = pin_rst;
  _pin_clk = pin_clk;
  _pin_dat = pin_dat;
  _dat_direction = INPUT;
  
  // RST
  gpio_init(_pin_rst);
  pinMode(_pin_rst, OUTPUT);
  gpio_set_function(_pin_rst, GPIO_FUNC_SIO);
  digitalWrite(_pin_rst, LOW);
  
  // CLK
  gpio_init(_pin_clk);
  pinMode(_pin_clk, OUTPUT);
  gpio_set_function(_pin_clk, GPIO_FUNC_SIO);
  digitalWrite(_pin_clk, LOW);
  
  // DAT
  gpio_init(_pin_dat);
  gpio_set_function(_pin_dat, GPIO_FUNC_SIO);
  pinMode(_pin_dat, _dat_direction);
}

int ds1302isHalted() {
  _prepareRead(REG_SECONDS);
  uint8_t seconds = _readByte();
  _end();
  return (seconds & 0b10000000);
}

void ds1302getDateTime(DateTime* dt) {
  _prepareRead(REG_BURST);
  dt->second = _bcd2dec(_readByte() & 0b01111111);
  dt->minute = _bcd2dec(_readByte() & 0b01111111);
  dt->hour   = _bcd2dec(_readByte() & 0b00111111);
  dt->day    = _bcd2dec(_readByte() & 0b00111111);
  dt->month  = _bcd2dec(_readByte() & 0b00011111);
  dt->dow    = _bcd2dec(_readByte() & 0b00000111);
  dt->year   = _bcd2dec(_readByte() & 0b01111111);
  _end();
}

void ds1302setDateTime(DateTime* dt) {
  // disable write protection
  _prepareWrite(REG_WP);
  _writeByte(0b00000000); // 0, false
  _end();
  // OR call writeProtect(0)

  _prepareWrite(REG_BURST);
  _writeByte(_dec2bcd(dt->second % 60 ));
  _writeByte(_dec2bcd(dt->minute % 60 ));
  _writeByte(_dec2bcd(dt->hour   % 24 ));
  _writeByte(_dec2bcd(dt->day    % 32 ));
  _writeByte(_dec2bcd(dt->month  % 13 ));
  _writeByte(_dec2bcd(dt->dow    % 8  ));
  _writeByte(_dec2bcd(dt->year   % 100));
  _writeByte(0b10000000);
  _end();
}

void ds1302halt() {
  _prepareWrite(REG_SECONDS);
  _writeByte(0b10000000);
  _end();
}

void _prepareRead(uint8_t address) {
  _setDirection(OUTPUT);
  digitalWrite(_pin_rst, HIGH);
  uint8_t command = 0b10000001 | address;
  _writeByte(command);
  _setDirection(INPUT);
}

void _prepareWrite(uint8_t address) {
  _setDirection(OUTPUT);
  digitalWrite(_pin_rst, HIGH);
  uint8_t command = 0b10000000 | address;
  _writeByte(command);
}

void _end() {
  digitalWrite(_pin_rst, LOW);
}

uint8_t _readByte() {
  uint8_t byte = 0;
  for(uint8_t b = 0; b < 8; b++) {
    if (digitalRead(_pin_dat) == HIGH) byte |= 0x01 << b;
    _nextBit();
  }
  return byte;
}

void _writeByte(uint8_t value) {
  for(uint8_t b = 0; b < 8; b++) {
    digitalWrite(_pin_dat, (value & 0x01) ? HIGH : LOW);
    _nextBit();
    value >>= 1;
  }
}

void _nextBit() {
  digitalWrite(_pin_clk, HIGH);
  delayMicroseconds(1);
  digitalWrite(_pin_clk, LOW);
  delayMicroseconds(1);
}

void _setDirection(int direction) {
  if (_dat_direction != direction) {
    _dat_direction = direction;
    pinMode(_pin_dat, direction);
  }
}

uint8_t _dec2bcd(uint8_t dec) {
  return ((dec / 10 * 16) + (dec % 10));
}

uint8_t _bcd2dec(uint8_t bcd) {
  return ((bcd / 16 * 10) + (bcd % 16));
}

// from https://www.arduino.cc/reference/en/
// https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Arduino.h

/**
 * Set the mode (input, output) for a specific GPIO pin.
 * @param pin ex: 16 17 18
 * @param mode input=false/0 output=true/1
 */
void pinMode(uint8_t pin, uint8_t mode) {
  gpio_set_dir(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t val) {
  gpio_put(pin, val);
}

int digitalRead(uint8_t pin) {
  return gpio_get(pin);
}

void delayMicroseconds(unsigned int us) {
  sleep_us(us);
}

// KSW  Store data to ds1302 RAM (7 bytes), and retrieve

/**
 * Get one byte of data from ds1302 RAM.
 * @param addr ex: REG_RAM30 0xFD ???
 */
void ds1302getRamByte(uint8_t addr, uint8_t *data) {
  _prepareRead(addr);
  *data = _readByte();
  _end();
}

/**
 * Set one byte of data in ds1302 RAM.
 * @param addr ex: REG_RAM30 0xFC ???
 * @param data ex: 0b10101010
 */
void ds1302setRamByte(uint8_t addr, uint8_t *data) {
  _prepareWrite(addr);
  _writeByte(*data);
  _end();
}

/**
 * first test failed; it printed 0
 * it works now after I disabled write protection
 */
void ds1302testRam() {
  uint8_t testaddrSET = 0xC0; //0xFC;
  uint8_t testdataSET = 0x30; // 0
  uint8_t testaddrGET = 0xC1; //0xFD;
  uint8_t testdataGET = 0x01;
  // get and display previously stored data (byte)
  ds1302getRamByte(testaddrGET, &testdataGET);
  uint8_t prevdata = testdataGET;
  testdataSET = prevdata + 1;
  ds1302setRamByte(testaddrSET, &testdataSET);
  ds1302getRamByte(testaddrGET, &testdataGET);
  printf("ds1302testRam prev:0x%x new:0x%x\n", prevdata, testdataGET);
}

// msparks
// this works - tested in ds1302pico,c
void writeProtect(int enable) {
  _prepareWrite(REG_WP);
  _writeByte(enable);
  _end();
}

// msparks
void writeRamBulk(const uint8_t* data, int len) {
  if (len <= 0) {return;}
  if (len > kRamSize) {len = kRamSize;}
  _prepareWrite(REG_RAM_BURST); // 0xFE
  for (int i = 0; i < len; i++) {
    _writeByte(data[i]);
  }
  _end();
}

// msparks
void readRamBulk(uint8_t* data, int len) {
  if (len <= 0) {return;}
  if (len > kRamSize) {len = kRamSize;}
  _prepareRead(REG_RAM_BURST); // TODO should be 0xFF ???
  for (int i = 0; i < len; i++) {
    data[i] = _readByte();
  }
  _end();
}
