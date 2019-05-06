#define DEBUGLEVEL 3
#include <Arduino.h>
#include "DebugUtils.h"   // using my own forked version

#include <Servo.h>  // by Michael Margolis
Servo myservo;      // create servo object to control a servo
#define servoPin 3

#include <avr/sleep.h>  // library for the low power sleep mode
#define wakePin 2       // interrupt pin to wake mcu up from deep sleep

#include <Keypad.h>   // matrix keypad library by Mark Stanley
const byte ROWS = 4;  //four rows      // keypad settings
const byte COLS = 3;  //three columns  // keypad settings
char keys[ROWS][COLS] = {
    // keypad settings
    {'1', '2', '3'},                  // keypad settings
    {'4', '5', '6'},                  // keypad settings
    {'7', '8', '9'},                  // keypad settings
    {'*', '0', '#'}                   // keypad settings
};                                    // keypad settings
byte rowPins[ROWS] = {11, 10, 9, 8};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5};       //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // starting the keypad object

#define TIMEOUT 30000   // 30 sec
#include <dwd.h>
Timer timer1(TIMEOUT);  // object for the inactivity timer set to 30 seconds
Timer timer2;
int t = 0;

#define passAddress 3            //  Start address of the current password string
#define badAddress 1             //  Start address of the bad attempts data in the eeprom
#include "password.h"            // Library for password stuff
password passObj1(badAddress);   // Object for the password
String str2;                     // used to hold the inputed password
int n = 0;                       // remember how many charecters inputed in the string of the password
int flag = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Function prototypes
void wakeUpNow(void);
void sleepNow(void);

void setup() {
    DEBUGSERIALBEGIN(9600);
    pinMode(wakePin, INPUT_PULLUP);  // setting wake up interrupt pin to input.
    myservo.attach(servoPin);        // attaches the servo on pin 11 to the servo object
    myservo.write(5);                // reset the servo

    DEBUGWHILENOTSERIAL;
    DEBUGPRINTLN0("Here We Go");

    passObj1.PP();

    if (!passObj1.is_ok()) {
        flag = 1;
        timer1.setDelta(TIMEOUT + 180000);
    }
    
    timer1.resetTimer();
    timer2.resetTimer();
}

void loop() {
    char key = customKeypad.getKey();

    if (key != NO_KEY) {
        DEBUGPRINT2("Key pressed: ");
        DEBUGPRINTLN2(key);

        if (passObj1.is_ok()) {  // mtObj1.is_ok()
            str2.concat(key);
            DEBUGPRINT2("Inputed Password string is: ");
            DEBUGPRINTLN2(str2);
            n++;
            if (n >= 33) {  // protection for crashing the processor with too long password
                str2 = String("");
                n = 0;
            }

            if (key == '#') {
                if (passObj1.checkPass(str2)) {
                    DEBUGPRINTLN1("password correct");
                    DEBUGPRINTLN1("Opening Door");

                    str2 = String("");
                    n = 0;
                    passObj1.resetBad();

                    myservo.write(90);
                    delay(10000);
                    myservo.write(5);
                    delay(3000);
                    DEBUGPRINTLN1("Door locked again. going to sleep at: ");

                } else {
                    DEBUGPRINTLN1("password wrong");

                    str2 = String("");  // clear the password input string. waiting for the next input.
                    n = 0;              // zero the counter of how many characters inputed
                    flag = passObj1.updateBad();     // updates the EEProm about the faild access attempt

                    DEBUGPRINTLN1("NOT Opening Door!!");
                }
            }

            if (flag) {
                timer1.setDelta(TIMEOUT + 180000);
                timer1.resetTimer();
                t = 0;
            } else {
                DEBUGPRINTLN0("Resetting runout timer after key press");
                timer1.resetTimer();
                t = 0;
            }
            
        }
    }

    // if (!digitalRead(wakePin)) {
    //   Serial.println("BP");
    // }

    if (timer1.checkInterval()) {  // if inactivity timer run out of time,
        if (flag) {
            flag = 0;
            timer1.setDelta(TIMEOUT);
            passObj1.resetBad();
        }

        DEBUGPRINTLN0("Inactivity runout");
        t = 0;
        sleepNow();  // put system to sleep
    } else {
        if (timer2.checkInterval()) {
            DEBUGPRINT3("timer2 click: ");
            DEBUGPRINTLN3(t);
            t++;
            timer2.resetTimer();
        }
    }
}

void wakeUpNow()  // here the interrupt is handled after wakeup
{
    // Prevent sleep mode, so we don't enter it again, except deliberately, by code
    sleep_disable();

    // Detach the interrupt that brought us out of sleep
    detachInterrupt(digitalPinToInterrupt(wakePin));
}

void sleepNow()  // here we put the arduino to sleep
{
    // Send a message just to show we are about to sleep
    DEBUGPRINTLN2("Good night!");
    Serial.flush();
    //     DEBUGSERIALFLUSH;
    // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;

    /* Set the type of sleep mode we want. Can be one of (in order of power saving):
         SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
         SLEEP_MODE_ADC
         SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
         SLEEP_MODE_EXT_STANDBY
         SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
         SLEEP_MODE_PWR_DOWN (Deep sleep)
     */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Turn of Brown Out Detection (low voltage)
    // Thanks to Nick Gammon for how to do this (temporarily) in software rather than
    // permanently using an avrdude command line.
    //
    // Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
    //
    // BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
    // the MCU Control Register (MCUCR)
    MCUCR = bit(BODS) | bit(BODSE);

    // The BODS bit is automatically cleared after three clock cycles so we better get on with it
    MCUCR = bit(BODS);

    // Ensure we can wake up again by first disabling interupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(wakePin), wakeUpNow, LOW);

    // Allow interrupts now
    interrupts();

    // And enter sleep mode as set above
    sleep_cpu();
    //sleep_mode();

    // --------------------------------------------------------
    // µController is now asleep until woken up by an interrupt
    // --------------------------------------------------------
    // Re-enable ADC if it was previously running
    ADCSRA = prevADCSRA;

    // Wakes up at this point when wakePin is brought LOW - interrupt routine is run first
    DEBUGPRINTLN2("PB press interrupt wakeup.");
    DEBUGPRINTLN2("I'm awake!");

     // Resetting bad attempts timer
    // passObj1.resetTime();
    // Resetting inactivity timer
    timer1.resetTimer();
}
