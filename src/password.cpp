#include "password.h"
#include <EEPROM.h>

password::password(byte Address) {
    EEPROM_Bad_Address = Address;
    badAttempts = EEPROM.read(EEPROM_Bad_Address);
    badTrials = EEPROM.read(EEPROM_Bad_Address + 1);

    EEPROM_ADDRESS = Address + 2;
    pass1 = String("");

    int i = EEPROM_ADDRESS;
    unsigned char uch = EEPROM.read(i);
    i++;
    int count = i + uch;
    for (; i < count; i++) {
        char ch = EEPROM.read(i);
        pass1.concat(ch);
    }

    timer.setDelta(180000);

    if (badAttempts >= maxAttempts) {
        timer.resetTimer();
    }
    
}

bool password::checkPass(String pass2) {
    return pass1.equals(pass2);
}

bool password::is_ok(){
    if (badAttempts < maxAttempts) {
        return 1;
    } else {
        if (timer.checkInterval()) {
            badAttempts = 0;
            EEPROM.write(EEPROM_Bad_Address,badAttempts);
            return 1;
        }
    }
    return 0;
}
    
bool password::updateBad(){
    badTrials ++;
    EEPROM.write(EEPROM_Bad_Address + 1,badTrials);
    
    badAttempts++;
    EEPROM.write(EEPROM_Bad_Address,badAttempts);
    if (badAttempts >= maxAttempts) {
        timer.resetTimer();
        return 1;
    }

    return 0;
}

void password::resetBad(){
    badAttempts = 0;
    badTrials = 0;
    EEPROM.write(EEPROM_Bad_Address,0);
    EEPROM.write(EEPROM_Bad_Address + 1,0);
}

void password::resetTime() {
    timer.resetTimer();
}

void password::PP() {  // this is only for debugging
    Serial.print("Password is: ");
    Serial.println(pass1);
}
