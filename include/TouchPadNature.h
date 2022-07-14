#include <Arduino.h>

class TouchPad
{
    //Analog pin connected
    int m_pin;
    // The status of the pad IE nt on note off
    boolean m_noteOn; 

    public:
    //Constructor
    TouchPad(){
        //Establish Deafaults
        m_noteOn=false;
        m_pin = 0;
    }

      //Setters and getters
  void setPin(int p)
  {
    m_pin = p;  
  }
  
  void setNoteOnStatus(boolean note_status)
  {
     m_noteOn=note_status; 
  }
  
  boolean getNoteOnStatus()
  {
     return m_noteOn; 
  }
  
  int getPadValue()
  {
    return touchRead(m_pin);
  } 
  
};