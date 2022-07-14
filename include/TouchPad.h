/*******************************************************************
 * HEXCHAIN
 Callum B, 2022

 FILE: CTouchPad.h

 DESCRIPTION: Class to capacitive touch pads

 USAGE: Teensy LC pins used

 OTHER: CRotaryEncoder will enable a pullup resistors for the given pins.

 *******************************************************************/
#ifndef _TOUCHPAD
#define _TOUCHPAD

// header file defines INPUT and HIGH
#include "Arduino.h"

class TouchPad
{
    // private:
public:
    int m_pin;        // analog pin the touch pad is connected to
    boolean m_noteOn; // the status of the pad (note on or note off)

    // Constructor
public:
    TouchPad()
    {
        // establish defaults
        m_noteOn = false;
        m_pin = 0;
    }

    // Setters and Getters
    void setPin(int p)
    {
        m_pin = p;
    }

    void setNoteOnStatus(boolean note_status)
    {
        m_noteOn = note_status;
    }

    boolean getNoteOnStatus()
    {
        return m_noteOn;
    }

    int getPadValue()
    {
        return analogRead(m_pin);
    }
};

#endif
