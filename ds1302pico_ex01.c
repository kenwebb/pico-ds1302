/**
 * ds1302 example
 *
 * @version 1.0.0
 * @author Ken Webb <ken@primordion.com>
 * @license MIT
 * @see https://github.com/kenwebb/
 * 
 * Raspberry Pi Pico - ds1302 C code
 * 27 Oct 2022
 * adapted from Arduino libraries by Ken Webb
 * This is an example app with main() function.
 * It uses the adapted Arduino-based libraries.
 * 
 * To compile, upload, observe (from Raspberry Pi 4):
 * cd TO_BUILD_FOLDER
 * cmake ..
 * make
 * openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ds1302pico_ex01.elf verify reset exit"
 * minicom -b 115200 -o -D /dev/serial0
 * minicom -b 115200 -o -D /dev/ttyACM0
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ds1302.h"

#define PIN_GPIO_CLK 18
#define PIN_GPIO_DAT 17
#define PIN_GPIO_RST 16

int main() {
  stdio_init_all();
  sleep_ms(2000); // the initial printf() only work if I have this initial 2000 ms delay
  //printf("starting ds1302pico ...\n"); // does not work
  printf("\nPico - ds1302 - Date Time DOW\n");
  
  //gpio_init(25);
  //gpio_set_dir(25, GPIO_OUT);
  
  ds1302init(PIN_GPIO_RST, PIN_GPIO_CLK, PIN_GPIO_DAT);
  writeProtect(0); // disable write protection
  
  DateTime now;
  
  // initialize time on ds1302; ONLY DO THIS ONCE
  /*now.year   = 22;
  now.month  = MONTH_OCT;
  now.day    = 29;
  now.hour   = 9;
  now.minute = 59;
  now.second = 0;
  now.dow    = DOW_SAT;*/
  //ds1302setDateTime(&now);
  
  // test bulk RAM ops - write (from msparks)
  static const char kHelloWorld[] = "abcdefghijklmnopqrstuvwxyzABCDEF"; // 31 chars; anything after E gets chopped off
  
  // test RAM ops
  ds1302testRam();
  
  // test RAM ops - write hex (from msparks)
  uint8_t testaddrGET = 0xC1;
  uint8_t testdataGET;
  for (int i = 0; i < kRamSize*2; i += 2) {
    ds1302getRamByte(testaddrGET + i, &testdataGET);
    printf("%02X ", testdataGET);
  }
  printf("\n");
  
  // test bulk RAM ops - write ASCII (from msparks)
  uint8_t ram[kRamSize];
  readRamBulk(ram, kRamSize);
  printf("%s\n", (const char*)ram);
  
  while (true) {
    sleep_ms(1000); // one second
    ds1302getDateTime(&now);
    //printf("DateTime second %d\n", now.second); // OK 0-59
    char* dow = "Noneday";
    switch (now.dow) {
      case DOW_MON: dow = "Monday"; break;
      case DOW_TUE: dow = "Tuesday"; break;
      case DOW_WED: dow = "Wednesday"; break;
      case DOW_THU: dow = "Thursday"; break;
      case DOW_FRI: dow = "Friday"; break;
      case DOW_SAT: dow = "Saturday"; break;
      case DOW_SUN: dow = "Sunday"; break;
      default: break;
    }
    printf("%02d/%02d/20%d %02d:%02d:%02d %s\n", now.month, now.day, now.year,
      now.hour, now.minute, now.second,
      dow);
  }
  
  return 0;
}
