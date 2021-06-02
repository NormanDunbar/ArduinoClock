/* RTCClockSetter
 * ==============
 *  
 * A sketch to allow a DS3231 RTC module to have the time set
 * prior to proper use in another sketch.
 * 
 * Make sure the backup battery is installed first though!
 *
 *
 * WIRING DIAGRAM
 * ==============
 *  
 *  ARDUINO     RTC
 *  ---------------
 *      A4      SDA
 *      A5      SCL
 *      5V      VCC
 *     GND      GND
 *     
 *  See: https://www.circuitbasics.com/wp-content/uploads/2020/05/arduino-rtc-wiring-2-768x298.png
 *  
 *  The updateRTC() function below is a greatly modified version of the code at:
 *  https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino
 *  and was modified by Norman Dunbar to allow the user to abort the setting and to
 *  validate the user's input for correctness.
 */


#include <RTClib.h>     // for RTCLib


// Minimum and maximum values for each input.
typedef struct minMax_t {
    int minimum;
    int maximum;
};



RTC_DS3231 rtc;     // create rtc for the DS3231 RTC module, address is fixed at 0x68.





/*
 * Function to validate user input.
 */
bool checkInput(const int value, const minMax_t minMax) {
    if ((value >= minMax.minimum) &&
        (value <= minMax.maximum))
        return true;

    Serial.print(value);
    Serial.print(" is out of range ");
    Serial.print(minMax.minimum);
    Serial.print(" - ");
    Serial.println(minMax.maximum);
    return false;
}


/*
   function to update RTC time using user input
*/
void updateRTC()
{  
    Serial.println("\nSetting date and time...");

    // Prompts for user input.
    const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                              "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};

    
    // Define limits for the 6 user inputs. Years between 2000 and 2099
    // are all that is permitted by the RTClib library.
    const minMax_t minMax[] = {
        {2000, 2099},   // Year
        {1, 12},        // Month
        {1, 31},        // Day
        {0, 23},        // Hour
        {0, 59},        // Minute
        {0, 59},        // Second
    };
    
    String str = "";
    
    long newDate[6];
    DateTime newDateTime;

    // The outer loop. Goes around and around until we get a valid
    // date and time.
    while (1) {
        while (Serial.available()) {
            Serial.read();  // clear serial buffer
        }

        // We have 6 different user inputs to capture.
        for (int i = 0; i < 6; i++) {

            // This loop exits when each user input is valid
            // as far as bieng numeric and in range, sort of. 
            // Leap years and month end dates are validated later
            // when we have the whole date and time.
            while (1) {
                Serial.print("Enter ");
                Serial.print(txt[i]);
                Serial.print(" (or -1 to abort) : ");
            
                while (!Serial.available()) {
                    ; // wait for user input
                }
        
                str = Serial.readString();  // read user input
        
                // The actual value depends on the line ending configured in Serial Monitor.
                // The configured line end character(s) are part of the input!
                if ((str == "-1")   || (str == "-1\n") || 
                    (str == "-1\r") || (str == "-1\r\n")) {
                        Serial.println("\nABORTED");
                        return;
                    }
                    
                newDate[i] = str.toInt();   // convert user input to number and save to array
                
                if (checkInput(newDate[i], minMax[i]))
                    break;
    
            }
            
            Serial.println(newDate[i]); // show user input
        }
    
        // We have all the user input, was it valid - leap years, days in months etc.
        newDateTime = DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
        if (newDateTime.isValid())
            break;            

        Serial.println("Date/time entered was invalid, please try again.");
    }
    
    // update RTC we have a valid date & time.
    rtc.adjust(newDateTime);
    Serial.println("RTC Updated!");
}


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
    char timeBuffer[] = " hh:mm:ss AP";
    
    // move LCD cursor to upper-left position
    Serial.print(rtcTime.toString(dateBuffer));

    // move LCD cursor to lower-left position
    Serial.println(rtcTime.toString(timeBuffer));
}


void setup()
{
    Serial.begin(9600); // initialize serial
    Serial.println("Enter 'u' to update the date and time.");
    rtc.begin();       // initialize rtc
}


void loop()
{
    updateLCD();  // update LCD text
    
    if (Serial.available()) {
        char input = Serial.read();
        if (input == 'u') updateRTC();  // update RTC time
    }

    delay(1000);
}
