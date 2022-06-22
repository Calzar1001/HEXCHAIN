#include "EasyButton.h"

EasyButton::EasyButton()
{
    //Set default values
    m_lastEventTime = 0; 
    m_pin = 0;
    m_debounceMS = 200;      
    m_currentState = false;    
};

 
void EasyButton::init(int pin, int debounce_milliseconds)
{
   m_pin = pin;
   m_debounceMS = debounce_milliseconds; 
   pinMode(m_pin, INPUT_PULLUP); //turn on internal pullup  
}
    
//Call this to see if the button is being pressed
bool EasyButton::checkButtonPressed()
{
   boolean state;
   if(digitalRead(m_pin) == LOW)
   {
       state = true;
   }else{
       state = false; 
   }
   
   //See if the state has changed
   if(state != m_currentState)
   {
        //See if enough time has passed to change the state 
        if((millis() - m_lastEventTime) > m_debounceMS)
        {
             //Okay to change state:
             m_currentState = state;
            //Reset time count
            m_lastEventTime = millis(); 
        }
   }
   
   return m_currentState;
}

bool EasyButton::checkButtonPressed(unsigned long ticks)
{
    unsigned long total_ticks=ticks-m_lastEventTime;
    if(digitalRead(m_pin)==LOW && total_ticks > m_debounceMS)
    {
        //Button was pressed...reset timer
        m_lastEventTime = ticks;
        return true;
    }
    return false;
}