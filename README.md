# pico-ds1302
Raspberry Pi Pico library for the DS1302 Real Time Clock chip

This project is a C library for the [Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html) platform. It
provides a simple interface to the
[Maxim DS1302](https://datasheets.maximintegrated.com/en/ds/DS1302.pdf)
timekeeping chip. It allows Pico projects to keep accurate time.

## Features

This library supports:

* Setting and retrieving the time, using the burst mode.
* Enabling and disabling the Write Protect (WP) flag.
* Setting and clearing the Clock Halt (CH) flag.
* Setting and getting the 31 bytes of static RAM, single-byte and multi-byte (burst) modes.

## Examples and documentation

The [header file](ds1302.h) is commented, and the directory includes an example ds1302pico_ex01.c .

## Raspberry Pi Pico - Pinout

```
Pico          DS1302 Device
------------- -------------
VSYS (PIN 39) VCC
GND  (PIN 38) GND
GP18 (PIN 24) CLK
GP17 (PIN 22) DAT
GP16 (PIN 21) RST
```

See the [photo](images/ds1302pico.jpg) for details.

## Installing, Compiling, Running the software

I assume you have the Pico build software installed and configured as described in the official Raspberry Pi Pico documentation.
In a terminal window, on a Raspberry Pi or other linux computer:
```
cd MYHOME/MYDIR
mkdir ds1302pico
cd ds1302pico
```
Copy the files from github to this folder
```
ls -l
```
You should see something like (note that you'll need to get the .cmake file from your Pico installation):
```
-rw-r--r-- 1 ken ken  438 Oct 27 13:47 CMakeLists.txt
-rw-r--r-- 1 ken ken 6976 Nov  1 08:18 ds1302.c
-rw-r--r-- 1 ken ken 3756 Nov  1 08:21 ds1302.h
-rw-r--r-- 1 ken ken 3537 Nov  1 08:34 ds1302pico_ex01.c
-rw-r--r-- 1 ken ken 2763 Jan 26  2022 pico_sdk_import.cmake
-rw-rw-r-- 1 ken ken 2111 Nov  1 10:35 README.md

```
Then do:
```
mkdir build
cd build
cmake ..
make
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ds1302pico_ex01.elf verify reset exit"
```
in a separate terminal window, do
```
minicom -b 115200 -o -D /dev/serial0
```
or
```
minicom -b 115200 -o -D /dev/ttyACM0
```

## Acknowledgments

I borrowed concepts and the Pico pinout from a
[Pico DS1302 Python](https://github.com/Guitarman9119/Raspberry-Pi-Pico-/tree/main/DS1302%20RTC)
project.

I borrowed and adapted ideas and code from these two Arduino projects:
[Rafa Couto](https://github.com/Treboada/Ds1302)
[Matt Sparks](https://github.com/msparks/arduino-ds1302)

DS1302 docs
[maxim integrated](https://datasheets.maximintegrated.com/en/ds/DS1302.pdf)
[DALLAS Semiconductor](https://www.solarbotics.com/wp-content/uploads/ds1302.pdf)
