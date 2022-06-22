//Arduino for Musicians
//Touch Pad class: 2012
#include <Arduino.h>

class TouchPad
{
  int m_pin;          //The analog pin to which the touch pad is attached
  boolean m_noteOn;   //The status of the pad (note on or note off)
  
  //Constructor
  public: 
  TouchPad()
  {
    //establish defaults
    m_noteOn=false;    //default to a note off
    m_pin=0;
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
    return analogRead(m_pin);
  } 
  
};
