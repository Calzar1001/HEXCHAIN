/*******************************************************************
 Arduino for Musicians
 Brent Edstrom, 2012
 
 FILE: EasyButton.h
 
 DESCRIPTION: Class to detect (and debounce) momentary switches.
 
 USAGE: Create an instance of the class and set the pin, debounce
 time (in milliseconds). Call checkButtonPressed() to check
 the state of the button: returns TRUE for a button press.
 Note that EasyButton will enable a pullup resistor for
 the given pin.
 
 *******************************************************************/
#ifndef _EASY_BUTTON
#define _EASY_BUTTON

#include "Arduino.h"  //Include header file that defines INPUT and HIGH


class EasyButton
{
private:
    int m_pin;                     //Digital pin the button is connected to
    int m_debounceMS;              //Number of milliseconds for debouncing
    unsigned long m_lastEventTime; //Timestamp of the last event
    boolean m_currentState;        //The current state of the button
    
public:
    //Constructor
    EasyButton();
    
    //Helper method to set pin and number of milliseconds for debounce.
    //Also sets up pullup register.
    void init(int pin, int debounce_milliseconds);
    
    //Call this to see if the button is being pressed
    bool checkButtonPressed();
    
    //Use this method when the millis() function is disabled. For example,
    //increment a counter in the main loop to generate "pseudo" time.
    bool checkButtonPressed(unsigned long ticks);
    
};

#endif



