# ArduinoClock
An Arduino clock using a 16 by 2 I2C/TWI Liquid Crystal Display (LCD) and a DS3231 Real Time Clock (RTC) module. 

The sketches use the following libraries:

* *TWILiquidCrystal* Library by Arnakazim.
* *RTCLib* by Adafruit.

**To install *TWILiquidCrystal***
* Sketch->Include Library->Manage Libraries.
* Filter for *TWILiquidCrystal*.
* Select the one that mentions Arnakazim.

**To install Adafruit's *RTCLib***
* Sketch->Include Library->Manage Libraries.
* Filter for *RTClib*.
* Select the one that mentions Adafruit.

The plan is to replace the Arduino UNO with an ATtiny85 and have buttons etc to adjust the date and time, turn on/off the backlight etc. Plans often come to naught though! :wink:

I might even make it sleep between updates and have the Watchdog wake up every second to update the LCD. ;)

This project is a variation of the one at [www.circuitbasics.com](https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino "https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino"). The image below is linked to that project.

[![An Arduino clock, using an RTC board based on the DS3231 RTC module.](https://www.circuitbasics.com/wp-content/uploads/2020/05/arduino-rtc-tutorial-1-1080x722.jpg "An Arduino clock, using an RTC board based on the DS3231 RTC module.")](https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino)
