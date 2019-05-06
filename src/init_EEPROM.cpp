////////////////////////////////////////////////
// This Scatch is used to initiate the EEPROM //
// For the Safe-2 project                     //
// github.com/RoiEf/Safe-2                    //
//                                            //
//  Roi Efrati                                //
//  6/5/2019                                  //
////////////////////////////////////////////////

#include <Arduino.h>
#include <EEPROM.h>

void setup() {
    int i = 1;
    int count;

    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Initialising EEPROM");

    Serial.print("Attempts address start: ");
    Serial.println(i);
    EEPROM.write(i++, 0);  // set attempts to 0
    EEPROM.write(i++, 0);  // set attempts to 0

    Serial.print("User password address start: ");
    Serial.println(i);
    EEPROM.write(i++, 6);  // set user password length 6
    EEPROM.write(i++, '1');
    EEPROM.write(i++, '2');
    EEPROM.write(i++, '3');
    EEPROM.write(i++, '4');
    EEPROM.write(i++, '5');
    EEPROM.write(i++, '#');

    count = i - 6 + 32;
    for (; i < count; i++) {
        EEPROM.write(i, '#');
    }

    Serial.print("Distributor password address start: ");
    Serial.println(i);
    EEPROM.write(i++, 6);  // set Distributor password length 6
    EEPROM.write(i++, '1');
    EEPROM.write(i++, '2');
    EEPROM.write(i++, '3');
    EEPROM.write(i++, '4');
    EEPROM.write(i++, '5');
    EEPROM.write(i++, '#');

    count = i - 6 + 32;
    for (; i < count; i++) {
        EEPROM.write(i, '#');
    }

    Serial.print("Factory password address start: ");
    Serial.println(i);
    EEPROM.write(i++, 32);  // set Factory password length 32
    EEPROM.write(i++, '1');
    EEPROM.write(i++, '2');
    EEPROM.write(i++, '3');
    EEPROM.write(i++, '4');
    EEPROM.write(i++, '5');
    EEPROM.write(i++, '6');
    EEPROM.write(i++, '7');
    EEPROM.write(i++, '8');
    EEPROM.write(i++, '9');
    EEPROM.write(i++, '0');
    EEPROM.write(i++, '0');
    EEPROM.write(i++, '9');
    EEPROM.write(i++, '8');
    EEPROM.write(i++, '7');
    EEPROM.write(i++, '6');
    EEPROM.write(i++, '5');
    EEPROM.write(i++, '4');
    EEPROM.write(i++, '3');
    EEPROM.write(i++, '2');
    EEPROM.write(i++, '1');
    EEPROM.write(i++, '1');
    EEPROM.write(i++, '2');
    EEPROM.write(i++, '3');
    EEPROM.write(i++, '4');
    EEPROM.write(i++, '5');
    EEPROM.write(i++, '6');
    EEPROM.write(i++, '7');
    EEPROM.write(i++, '8');
    EEPROM.write(i++, '9');
    EEPROM.write(i++, '0');
    EEPROM.write(i++, '0');
    EEPROM.write(i++, '9');
    EEPROM.write(i++, '#');

    Serial.print("Model / Serial address start: ");
    Serial.println(i);
    EEPROM.write(i++, 1);  // Model
    EEPROM.write(i++, 0);  // Serial No. MSB
    EEPROM.write(i++, 0);
    EEPROM.write(i++, 0);
    EEPROM.write(i++, 1);  // Serial No. LSB

    Serial.println("EEPROM was updated");
}

void loop() {
}
