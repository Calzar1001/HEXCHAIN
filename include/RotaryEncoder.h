/*******************************************************************
 Arduino for Musicians
 Brent Edstrom, 2012
 
 FILE: CRotaryEncoder.h
 
 DESCRIPTION: Class to track a rotary encoder
 
 USAGE: Connect the center pin of the encoder to ground
 and the outer pins to digital pins (e.g. digital pins 3 & 4)
 
 OTHER: CRotaryEncoder will enable a pullup resistors for the given pins.
 
 *******************************************************************/
#ifndef _ROTARYENCODER
#define _ROTARYENCODER

#include "Arduino.h"  //Include header file that defines INPUT and HIGH

class RotaryEncoder
{
//private:
public:
    int m_pinA;            //Digital pin encoder A is connected to.
    int m_encoderALast;    //Tracks the last value of encoder A
    int m_pinB;            //Digital pin encoder B is connected to.
    int m_debounceMS;      //Number of milliseconds for debouncing.
    unsigned long m_lastEventTime;   //Timestamp of the last event.
    int m_encoderPosition; //Current position of the encoder:
                           //between m_encoderMax and m_encoderMin.
    int m_encoderMax;      //Sets the maximum value of the encoder.
    int m_encoderMin;      //Sets the minimum value of the encoder.
    
public:
    //Constructor
    RotaryEncoder();
    
    void init(int pinA, int pinB, int debounce_milliseconds,
              int start_position = 0, int min_position = 0, int max_position = 100);
    
    //"Setters" and "getters"
    void setShaftPosition(int position);
    void setMinValue(int min);
    void setMaxValue(int max);
    
    //Returns value of m_encoderPosition--use trackShaftPosition()
    //to track changes
    int shaftPosition();
    
    //Call this in main loop() to track the position of the shaft
    int trackShaftPosition();
    
    //Call this in main loop() to track the position of the shaft
    //if the millis() function is disabled. For example, increment
    //a counter in the main loop() to generate "pseudo" time.
    int trackShaftPosition(unsigned long ticks);
    
    
};

#endif

