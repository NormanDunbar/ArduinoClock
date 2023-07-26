# Sketch Sources
There are a few different sketches in this directory. These are:

  * **ArduinoClock** - the original code from [https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino](URL) but with my modifications to the ``updateRTC()`` and ``updateLCD()`` functions. I've added better validation of user input and a much smaller display function using the features of the RTF library to do the LCD screen formatting.

  * **ArduinoClockWithPulsing** - a version of ArduinoClock, written for Mark Turner, which generates a 1 second pulse every 30 seconds. This allows for an interface to a master clock (see [https://www.britishtelephones.com/clocks/clock36.htm]) and uses an interrupt to count an alarm pulse from the clock module, which triggers every second, and to generate a pulse every 30 seconds.

  * **RTCClockSetter** - My version of the ``updateRTC()`` function extracted to a sketch of its own. My intention is to convert the original code to run on an ATtiny85 which doesn't have ``Serial`` so a separate sketch will be used to set the clock. This assumes that the clock module does have a backup battery fitted of course!

  * **RTC_Uno** - My version of the original code for an Arduino Uno, but without the ability to set the clock. That is done with the **RTCClockSetter** sketch. Serial is not used in this sketch.

  * **RTC_xxxx** - My version of **RTC_Uno** for different AVR microcontrollers. _Just in case_ I start creating other stuff and haven't updated this file yet.

