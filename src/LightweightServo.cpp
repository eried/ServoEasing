/*
 *  LightweightServo.cpp
 *
 *  Lightweight Servo implementation only for pin 9 and 10 using only timer1 hardware and no interrupts or other overhead.
 *  Provides auto initialization.
 *  300 bytes code size / 4 bytes RAM including auto initialization compared to 700 / 48 bytes for Arduino Servo library.
 *  8 bytes for each call to setLightweightServoPulse...
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
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>
#include "LightweightServo.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
/*
 * Outcommenting this saves 70 bytes flash memory. You must then use the init function initLightweightServoPin9And10() manually.
 */
//#define DISABLE_SERVO_TIMER_AUTO_INITIALIZE

#define COUNT_FOR_20_MILLIS 40000 // you can modify this if you have servos which accept a higher rate

/*
 * Variables to enable adjustment for different servo types
 * 544 and 2400 are values compatible with standard arduino values
 * 4 bytes RAM compared to 48 bytes for standard Arduino library
 */
int sMicrosecondsForServo0Degree = 544;
int sMicrosecondsForServo180Degree = 2400;

/*
 * Use 16 bit timer1 for generating 2 servo signals entirely by hardware without any interrupts.
 * Use FastPWM mode and generate pulse at start of the 20ms period
 * The 2 servo signals are tied to pin 9 and 10 of an 328.
 * Attention - both pins are set to OUTPUT here!
 * 32 bytes code size
 */
void initLightweightServoPin9And10() {
    /*
     * Periods below 20 ms gives problems with long signals i.e. the positioning is not possible
     */
    DDRB |= _BV(DDB1) | _BV(DDB2);                // set pins OC1A = PortB1 -> PIN 9 and OC1B = PortB2 -> PIN 10 to output direction
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11); // FastPWM Mode mode TOP (20ms) determined by ICR1 - non-inverting Compare Output mode OC1A+OC1B
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);    // set prescaler to 8, FastPWM mode mode bits WGM13 + WGM12
    ICR1 = COUNT_FOR_20_MILLIS;  // set period to 20 ms
    // do not set counter here, since with counter = 0 (default) no output signal is generated.
}

/*
 * Use 16 bit timer1 for generating 2 servo signals entirely by hardware without any interrupts.
 * Use FastPWM mode and generate pulse at start of the 20ms period
 * The 2 servo signals are tied to pin 9 and 10 of an 328.
 * Attention - the selected pin is set to OUTPUT here!
 * 54 bytes code size
 */
void initLightweightServoPin9_10(bool aUsePin9, bool aUsePin10) {
    /*
     * Periods below 20 ms may give problems with long signals i.e. the positioning is not possible
     */
    uint8_t tNewTCCR1A = TCCR1A & (_BV(COM1A1) | _BV(COM1B1)); // keep existing channel settings
    tNewTCCR1A |= _BV(WGM11); // FastPWM Mode mode TOP (20ms) determined by ICR1

    if (aUsePin9) {
        DDRB |= _BV(DDB1);   // set OC1A = PortB1 -> PIN 9 to output direction
        tNewTCCR1A |= _BV(COM1A1); // non-inverting Compare Output mode OC1A
    }
    if (aUsePin10) {
        DDRB |= _BV(DDB2);   // set OC1B = PortB2 -> PIN 10 to output direction
        tNewTCCR1A |= _BV(COM1B1); // non-inverting Compare Output mode OC1B
    }
    TCCR1A = tNewTCCR1A;
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);    // set prescaler to 8, FastPWM Mode mode bits WGM13 + WGM12
    ICR1 = COUNT_FOR_20_MILLIS;  // set period to 20 ms
    // do not set counter here, since with counter = 0 (default) no output signal is generated.
}

/*
 * If value is below 180 then assume degree, otherwise assume microseconds
 * If aUpdateFast then enable starting a new output pulse if more than 5 ms since last one, some servo might react faster in this mode.
 * If aUsePin9 is false, then Pin10 is used
 * 236 / 186(without auto init) bytes code size
 */
int writeLightweightServo(int aValue, bool aUsePin9, bool aUpdateFast) {
    if (aValue <= 180) {
        aValue = DegreeToMicrosecondsLightweightServo(aValue);
    }
    writeMicrosecondsLightweightServo(aValue, aUsePin9, aUpdateFast);
    return aValue;
}

void writeMicrosecondsLightweightServo(int aMicroseconds, bool aUsePin9, bool aUpdateFast) {
#ifndef DISABLE_SERVO_TIMER_AUTO_INITIALIZE
    // auto initialize
    if ((TCCR1B != (_BV(WGM13) | _BV(WGM12) | _BV(CS11))) || (aUsePin9 && ((TCCR1A & ~_BV(COM1B1)) != (_BV(COM1A1) | _BV(WGM11))))
            || (!aUsePin9 && ((TCCR1A & ~_BV(COM1A1)) != (_BV(COM1B1) | _BV(WGM11))))) {
        initLightweightServoPin9_10(aUsePin9, !aUsePin9);
    }
#endif
    // since the resolution is 1/2 of microsecond
    aMicroseconds *= 2;
    if (aUpdateFast) {
        uint16_t tTimerCount = TCNT1;
        if (tTimerCount > 10000) {
            // more than 5 ms since last pulse -> start a new one
            TCNT1 = COUNT_FOR_20_MILLIS - 1;
        }
    }
    if (aUsePin9) {
        OCR1A = aMicroseconds;
    } else {
        OCR1B = aMicroseconds;
    }
}

/*
 * Set the mapping pulse width values for 0 and 180 degree
 */
void setLightweightServoPulseMicrosFor0And180Degree(int aMicrosecondsForServo0Degree, int aMicrosecondsForServo180Degree) {
    sMicrosecondsForServo0Degree = aMicrosecondsForServo0Degree;
    sMicrosecondsForServo180Degree = aMicrosecondsForServo180Degree;
}

/*
 * Pin 9 / Channel A. If value is below 180 then assume degree, otherwise assume microseconds
 */
void write9(int aValue, bool aUpdateFast) {
    writeLightweightServo(aValue, true, aUpdateFast);
}

void writeMicroseconds9(int aMicroseconds, bool aUpdateFast) {
    writeMicrosecondsLightweightServo(aMicroseconds, true, aUpdateFast);
}
/*
 * Pin 10 / Channel B
 */
void write10(int aValue, bool aUpdateFast) {
    writeLightweightServo(aValue, false, aUpdateFast);
}

void writeMicroseconds10(int aMicroseconds, bool aUpdateFast) {
    writeMicrosecondsLightweightServo(aMicroseconds, false, aUpdateFast);
}

int DegreeToMicrosecondsLightweightServo(int aValueDegree) {
    return (map(aValueDegree, 0, 180, sMicrosecondsForServo0Degree, sMicrosecondsForServo180Degree));
}

int MicrosecondsToDegreeLightweightServo(int aValueMicros) {
    return map(aValueMicros, sMicrosecondsForServo0Degree, sMicrosecondsForServo180Degree, 0, 180);
}

#endif

