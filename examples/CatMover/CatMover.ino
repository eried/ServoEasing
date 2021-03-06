/*
 *  CatMover.cpp
 *  Demo of using two servos in a pan tilt housing to move a laser pointer
 *
 *
 *  Copyright (C) 2019  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of ServoEasing https://github.com/ArminJo/ServoEasing.
 *
 *  ServoEasing is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "ServoEasing.h"

#define VERSION_EXAMPLE "1.1"

#ifdef ESP8266
const int LASER_POWER_PIN = 13;

const int HORIZONTAL_SERVO_PIN = 14; // D5
const int VERTICAL_SERVO_PIN = 12; // D6
#else
const int LASER_POWER_PIN = 5;

// These pins are used by Timer 2 and can be used without overhead by using SimpleServo - not used yet!
const int HORIZONTAL_SERVO_PIN = 10;
const int VERTICAL_SERVO_PIN = 9;
#endif

struct ServoControlStruct {
    uint16_t minDegree;
    uint16_t maxDegree;
};
ServoControlStruct ServoHorizontalControl;
ServoControlStruct ServoVerticalControl;

ServoEasing ServoHorizontal;
ServoEasing ServoVertical;

void setup() {
// initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LASER_POWER_PIN, OUTPUT);
    digitalWrite(LASER_POWER_PIN, HIGH);

    Serial.begin(115200);
    while (!Serial)
        ; //delay for Leonardo
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from  " __DATE__));

    /*
     * Set up servos
     */
    ServoHorizontal.attach(HORIZONTAL_SERVO_PIN);
    ServoVertical.attach(VERTICAL_SERVO_PIN);

    ServoHorizontalControl.minDegree = 45;
    ServoHorizontalControl.maxDegree = 135;
    ServoVerticalControl.minDegree = 0;
    ServoVerticalControl.maxDegree = 45;

    // this helps mounting the pan / tilt housing
    ServoHorizontal.write(90);
    ServoVertical.write(90);

    delay(4000);

    /*
     * show border of area which can be reached by laser
     */
    Serial.println(F("Mark border of area and then do auto move."));
    ServoHorizontal.write(ServoHorizontalControl.minDegree);
    ServoVertical.write(ServoVerticalControl.minDegree);
    delay(500);
    analogWrite(LASER_POWER_PIN, 255);
    ServoHorizontal.easeTo(ServoHorizontalControl.maxDegree, 50);
    ServoVertical.easeTo(ServoVerticalControl.maxDegree, 50);
    ServoHorizontal.easeTo(ServoHorizontalControl.minDegree, 50);
    ServoVertical.easeTo(ServoVerticalControl.minDegree, 50);
}

uint8_t getRandomValue(ServoControlStruct * aServoControlStruct, ServoEasing * aServoEasing) {
    /*
     * get new different value
     */
    uint8_t tNewTargetAngle;
    do {
        tNewTargetAngle = random(aServoControlStruct->minDegree, aServoControlStruct->maxDegree);
    } while (tNewTargetAngle == aServoEasing->MicrosecondsOrUnitsToDegree(aServoEasing->mCurrentMicrosecondsOrUnits)); // do not accept current angle as new value
    return tNewTargetAngle;
}

void loop() {

    if (!ServoHorizontal.isMoving()) {
        /*
         * start new random move in the restricted area
         */
        delay(random(500));
        uint8_t tNewHorizontal = getRandomValue(&ServoHorizontalControl, &ServoHorizontal);
        uint8_t tNewVertical = getRandomValue(&ServoVerticalControl, &ServoVertical);
        int tSpeed = random(10, 90);

        Serial.print(F("Move to horizontal="));
        Serial.print(tNewHorizontal);
        Serial.print(F(" vertical="));
        Serial.print(tNewVertical);
        Serial.print(F(" speed="));
        Serial.println(tSpeed);
        ServoHorizontal.setEaseTo(tNewHorizontal, tSpeed);
        ServoVertical.setEaseTo(tNewVertical, tSpeed);
        synchronizeAllServosAndStartInterrupt();
    } else {
        /*
         * Do whatever you want in his part of the loop
         */

    }
}
