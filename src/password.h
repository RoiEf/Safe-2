#ifndef password1234
#define password1234

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// #include <EEPROM.h>
#include <dwd.h>

class password {
    String pass1;               // Hold the current user password
    byte EEPROM_ADDRESS;        // Address of the password length in EEPROM, ++ is start of the password itself
    byte EEPROM_Bad_Address;
    char badAttempts = -1;      // remembering up to 3 attempts before relock.
    char badTrials = -1;        // remembering the number of times the wrong password was inputed.
    byte maxAttempts = 3;
    byte maxTrials = 5;
    Timer timer;        // base setting 3 min

   public:
    password(byte Address);
    bool checkPass(String);
    bool is_ok();
    bool updateBad();
    void resetBad();
    void resetTime();
    
    void PP();
};

#endif
