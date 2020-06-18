#include <Wire.h>                   // for I2C communication
#include <LiquidCrystal_I2C.h>      // for LCD
#include <RTClib.h>                 // for RTC

LiquidCrystal_I2C lcd(0x3f, 16, 2); // create LCD with I2C address 0x3F, 16 characters per line, 2 lines
RTC_DS3231 rtc;                     // create rtc for the DS3231 RTC module, address is fixed at 0x68



/* WIRING DIAGRAM:
 *  
 *  ARDUINO     RTC     LCD DISPLAY
 *  -------------------------------
 *      A4      SDA         SDA
 *      A5      SCL         SCL
 *      5V      VCC         VCC
 *     GND      GND         GNG 
 *     
 *  See: https://www.circuitbasics.com/wp-content/uploads/2020/05/arduino-rtc-wiring-2-768x298.png
 *  
 *  The updateLCD() function below is a greatly modified version of the code at:
 *  https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino
 *  and was modified by Norman Dunbar to use built in RTCLib functions to
 *  format the date and time for display on the LCD.
 */
   
 
/*
   function to update LCD text
*/
void updateLCD()
{
    // get time and date from RTC and save in variables
    DateTime rtcTime = rtc.now();

    /*
     * Buffers to format the date and time (on separate lines of the LCD)
     * 
     * Parameters are:
     *
        | specifier | output                                                 |
        |-----------|--------------------------------------------------------|
        | YYYY      | the year as a 4-digit number (2000--2099)              |
        | YY        | the year as a 2-digit number (00--99)                  |
        | MM        | the month as a 2-digit number (01--12)                 |
        | MMM       | the abbreviated English month name ("Jan"--"Dec")      |
        | DD        | the day as a 2-digit number (01--31)                   |
        | DDD       | the abbreviated English day of the week ("Mon"--"Sun") |
        | AP        | either "AM" or "PM"                                    |
        | ap        | either "am" or "pm"                                    |
        | hh        | the hour as a 2-digit number (00--23 or 01--12)        |
        | mm        | the minute as a 2-digit number (00--59)                |
        | ss        | the second as a 2-digit number (00--59)                |
    
        If either "AP" or "ap" is used, the "hh" specifier uses 12-hour mode
        (range: 01--12). Otherwise it works in 24-hour mode (range: 00--23).
    
        The specifiers within _buffer_ will be overwritten with the appropriate
        values from the DateTime. Any characters not belonging to one of the
        above specifiers are left as-is.
     */
     
    char dateBuffer[] = "DD-MMM-YYYY DDD";
    char timeBuffer[] = "hh:mm:ss AP";
    
    // move LCD cursor to upper-left position
    lcd.setCursor(0, 0);
    lcd.print(rtcTime.toString(dateBuffer));

    // move LCD cursor to lower-left position
    lcd.setCursor(0, 1);
    lcd.print(rtcTime.toString(timeBuffer));
}


void setup()
{
    Serial.begin(9600); // initialize serial
    
    lcd.init();        // initialize lcd
    lcd.backlight();    // switch-on lcd backlight
  
    rtc.begin();        // initialize rtc
}


void loop()
{
    updateLCD();    // update LCD text
    delay(1000);    // No point being too quick!
}


