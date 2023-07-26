/*
 * This sketch uses the I2C Liquid Crystal Library by Arnakazim.
 *
 * To install: 
 *   Sketch->Include Library->Manage Libraries.
 *   Filter for "TWILiquidCrystal" without quotes.
 *   Select the one that mentions Arnakazim.
 *
 * To install Adafruit's RTCLib:
 *   Sketch->Include Library->Manage Libraries.
 *   Filter for "RTClib" without quotes.
 *   Select the one that mentions Adafruit.
 */

/*===========================================================================
 * MARK TURNER
 *
 * All code relavant to your need for a 1 second pulse every 30 seconds
 * is bracketed with "Mark T begin" and "Mark T end" -- in case you want
 * to examine what I've done. ;-)
 *
 * The code pulses pin 8. It uses direct port access instead of digitalWrite
 * as digitalWrite takes all day to do what it needs to do. Direct access is
 * orders of magnitude faster. Timing could be critical. If you change it, 
 * you need to change the PORTx name and the pin within, PORTxn where n is
 * the pin number.
 *
 * Variables:
 *      interruptCount: Counts the interrupts from the square wave pin on the
 *                      RTC module. Every PULSE_EVERY_SECONDS counts it will
 *                      toggle LED_BUILTIN for PULSE_LENGTH_MILLIS milli-
 *                      seconds.
 *
 *      rtcInterruptPin: The pin, which must be 2 or 3 only, where the square
 *                       wave pin on the RTC module is connected. Every time 
 *                       the square wave is rising from low to high, it will
 *                       trigger an interrupt whcih increments interruptCount.
 *
 *      PULSE_LENGTH_MILLIS: How many milliseconds should the pulse last?
 *
 *      PULSE_EVERY_SECONDS: How often, in seconds, should there be a pulse?
 *
 *      pulsingInProgress: Are we currently pulsing?
 *
 *      savedMillis: When did we start pulsing the current pulse?
 *===========================================================================*/

// Mark T begin
// Interrupt counter. Counts seconds from the RTC.
// Maximum count is 255.
volatile uint8_t interruptCount = 0;

// Which pin, 2 or 3, detects the RTC pulses every second.
const uint8_t rtcInterruptPin = 2;

// How long, in milliseconds, should a pulse last.
// Maximum length is 65,535 millis.
const uint16_t PULSE_LENGTH_MILLIS = 1000;

// How oftwen, in seconds, should a pulse be triggered.
// Maximum is 255 seconds.
const uint8_t PULSE_EVERY_SECONDS = 30;

// Are we pusling at the moment flag.
bool pulsingInProgress = false;

// When did the pulse start.
uint32_t savedMillis = 0;
// Mark T begin


#include <TwiLiquidCrystal.h>       // For LCD. Pulls in "Wire.h" also.
#include <RTClib.h>                 // For RTC Library.

// Definitions for the LCD.
const uint8_t LCD_ADDRESS = 0x27;
const uint8_t LCD_ROWS = 2;
const uint8_t LCD_COLS = 16;

TwiLiquidCrystal lcd(LCD_ADDRESS); 
RTC_DS3231 rtc;  // Create RTC for the DS3231 RTC module, address is 0x68 fixed.

/* WIRING DIAGRAM:
 *  
 *  ARDUINO     RTC     LCD DISPLAY
 *  -------------------------------
 *      A4      SDA         SDA
 *      A5      SCL         SCL
 *      5V      VCC         VCC
 *     GND      GND         GND 
 *     
 *  
 *  The code below is a modified version of the code at:
 *  https://www.circuitbasics.com/how-to-use-a-real-time-clock-module-with-the-arduino
 *  and was modified by: 
 *  
 *  Norman Dunbar [norman (at) dunbar-it (dot) co (dot) uk] to: 
 *  
 *  Use built in RTCLib functions to
 *  format the date and time for display on the LCD.
 *  
 *  carry out validation checks on the user input and 
 *  the DateTime created from same, before updating the RTC.
 *  
 *  
 *  USAGE:
 *  
 *  The clock runs happily without problems, as long as it has 5V down the USB!
 *  
 *  Open the serial monitor, or similar, and type u (and enter). The LCD will display "Edit mode"
 *  and you will be prompted to enter the years, months etc to set the clock.
 *  
 *  While entering details, if you need to abort, type -1 for the value and all changes so 
 *  far will be aborted. The LCD will return to showing the date and time as before.
 *  
 *  All inputs are validated for a certain range of values. Months are 1 to 12, but what about
 *  leap years? Those are not validated (yet) and neither are "30 days hath September ...".
 *  
 *  After all inputs are done, a temporary DateTime is created with the entered values. If 
 *  this is a valid date time - leap year ok, days in month ok etc - then the RTC will be
 *  set. If the DateTime is invalid, then we go around all the inputs again.
 *  
 */

// Minimum and maximum values structure for each input.
typedef struct minMax_t {
    int minimum;
    int maximum;
} minMax_t;


/*
 * Function to validate user input.
 * Returns TRUE if value in range, FALSE otherwise.
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
   Function to update RTC time using user input.
*/
void updateRTC()
{
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Edit Mode...");

    // Prompts for user input.
    const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                              "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};

    
    // Define limits for the 6 user inputs.
    const minMax_t minMax[] = {
        {2000, 9999},   // Year
        {1, 12},        // Month
        {1, 31},        // Day
        {0, 23},        // Hours
        {0, 59},        // Minutes
        {0, 59},        // Seconds
    };
    
    String str = "";
    
    long newDate[6];
    DateTime newDateTime;

    // The outer loop. Goes around and around until we get a valid
    // date and time. Thats all 6 inputs valid. It does not validate
    // February and/or leap years - it doesn't have to DateTime.isValid()
    // does that for us.
    while (1) {
        while (Serial.available()) {
            Serial.read();  // Clear serial buffer
        }

        // We have 6 different user inputs to capture.
        for (int i = 0; i < 6; i++) {

            // This loop exits when one user input is valid in
            // as far as being numeric and in range, sort of. 
            // Leap years and month end dates are validated later
            // when we have the complete date and time.
            while (1) {
                Serial.print("Enter ");
                Serial.print(txt[i]);
                Serial.print(" (or -1 to abort) : ");
            
                while (!Serial.available()) {
                    ; // Wait for user input
                }
        
                str = Serial.readString();  // Read user input
        
                // The actual value depends on the line ending configured in 
                // the Serial Monitor. The configured line end character(s) 
                // are part of the input string!
                // If the value is -1, then we abort the clock change
                // completely and bale out of this function.
                if ((str == "-1")   || (str == "-1\n") || 
                    (str == "-1\r") || (str == "-1\r\n")) {
                        Serial.println("\nABORTED");
                        return;
                    }
                    
                newDate[i] = str.toInt();   // Convert user input to number and save to array

                // Validate input is in range, exit this inner while() loop
                // if so, otherwise, lets go round again.
                if (checkInput(newDate[i], minMax[i]))
                    break;
    
            }
            
            Serial.println(newDate[i]); // Show user their input
        }
    
        // We have all the user input, was it valid - leap years, 
        // days in months etc. If the DateTime is valid, we are done
        // in the outer while() loop and will exit to set the RTC.
        newDateTime = DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
        if (newDateTime.isValid())
            break;            

        // Otherwise, we have to do it all again.
        Serial.println("Date/time entered was invalid, please try again.");
    }
    
    // Update RTC as we have a valid date & time.
    rtc.adjust(newDateTime);
    Serial.println("RTC Updated!");
}

/*
   Function to update LCD text
*/
void updateLCD()
{
    // Get time and date from RTC.
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
    char timeBuffer[] = "hh:mm:ss";
    
    // Move LCD cursor to top line, far left position.
    lcd.setCursor(0, 0);
    lcd.print(rtcTime.toString(dateBuffer));

    // Move LCD cursor to second line, far left position.
    lcd.setCursor(0, 1);
    lcd.print(rtcTime.toString(timeBuffer));
}


void setup()
{
    Serial.begin(9600);
    lcd.begin(LCD_COLS, LCD_ROWS);       // Initialize lcd
    lcd.backlight();    // Switch-on lcd backlight
    lcd.clear();
    rtc.begin();        // Initialise RTC module

// Mark T begin
    // We need to pulse this pin, so it's OUTPUT.
    pinMode(8, OUTPUT);

    // The RTC needs a pullup resistor in the square wave pin.
    pinMode(rtcInterruptPin, INPUT_PULLUP);

    // Make sure we count the seconds.
    attachInterrupt(digitalPinToInterrupt(rtcInterruptPin), rtcInterrupt, RISING);

    // Enable the square wave output signal to 1 Hz.
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
// Mark T end

}


void loop()
{
// Mark T begin
    // As timings are important, possigly critical, we need to be
    // checking to see if we need to start or stop a pulse first,
    // before we bother with displaying the date and time.



    // If we are pulsing, check if it is time to stop. Use
    // direct port access to stop the pulse as this is much
    // much ,much faster than digitalWrite().
    if (pulsingInProgress) {
        if (millis() - savedMillis >= PULSE_LENGTH_MILLIS) {
            PORTB &= ~(1 << PORTB0);    // fast digitalWrite();
            pulsingInProgress = false;
        }
    } else {
        // We are not pulsing yet. Should we be?
        // If so, save the start time, use direct port access to
        // turn on the pulse pin, reset the interrupt counter and
        // kep a note that we are currently pulsing.
        if (interruptCount >= PULSE_EVERY_SECONDS) {
            savedMillis = millis();
            PORTB |= (1 << PORTB0);    // fast digitalWrite();
            interruptCount = 0;
            pulsingInProgress = true;
        }
    }
// Mark T end


    // Regardless of whether we are pulsing or not, display the curent date and time. 
    updateLCD();


    if (Serial.available()) {
        char input = Serial.read();
        if (input == 'u') updateRTC();
    }
}


// Mark T begin
void rtcInterrupt(void)
{
    interruptCount++;
}
// Mark T end

