#include <Arduino.h>
#include "TouchPadNature.h"

//Operation mode constants
const int PLAY_MODE=0;

//Touch pad constants
const int NUM_PADS=6;
const int NO_PAD_SELECTED=-1;
//Top row pin assignments
const int fsrA=0;
const int fsrB=1;
const int fsrC=3;
const int fsrD=4;
const int fsrE=15;
const int fsrF=16;

//Range values
const int minimum=0;
const int maximum=1000;
//Consider adding code for user to adjust the threshold value via the rotary encoder:
int THRESHOLD=100; 

//Variables to track the currently active pad and bank
int current_pad=0;
int current_bank=0;
//this variable indicates the current mode of operation
int currentMode=PLAY_MODE;


byte MIDI_NOTES[6][6]={
  42, 46, 44, 35, 38, 49,   /*drums */
  43, 45, 47, 48, 50, 54,   /*toms*/
  62 ,63, 64, 60, 61, 58,   /*conga*/
  65, 66, 67, 68, 69, 70,   /*timbale*/
  73, 73, 75, 76, 77, 78,   /*guiro*/
  37, 39, 51, 52, 57, 55};  /*cymbals*/
  
//Create an array of TouchPad objects
TouchPad TouchPads[NUM_PADS];



void setup()
{
  //set pin assignments
  TouchPads[0].setPin(fsrA);
  TouchPads[1].setPin(fsrB);
  TouchPads[2].setPin(fsrC);
  TouchPads[3].setPin(fsrD);
  TouchPads[4].setPin(fsrE);
  TouchPads[5].setPin(fsrF);

  //Delay to stabilize the circuitry
  // Not Sure if required?
  delay(500);

}

void loop()
{
     //Check the current mode status and call the corresponding method
    if(currentMode==PLAY_MODE)
    {
        playMode();
    }

}

void handleFSR(int index)
{
  //read FSR values, scale to midi velocity (0-127) and output as MIDI notes
  int val=TouchPads[index].getPadValue();
  int last_value=0;

  //Scale the FSR value to a valid MIDI velocity range
  int  midi_velocity=map(val, minimum, maximum, 0, 127);

  //===========Send a NOTE ON message if the given pad is currently off
  if(val>THRESHOLD && TouchPads[index].getNoteOnStatus()==false)
  {

    TouchPads[index].setNoteOnStatus(true);  

    //send a single note on for value greater than 0
    usbMIDI.sendNoteOn(MIDI_NOTES[current_bank][index],midi_velocity,1);
  }

  //===========Send a NOTE OFF message if the given pad is currently on
  if(midi_velocity==0 && TouchPads[index].getNoteOnStatus()==true)
  {
    //send a single note off for velocity of 0
    usbMIDI.sendNoteOff(MIDI_NOTES[current_bank][index],0,1);
    TouchPads[index].setNoteOnStatus(false); 
  }
}

void playMode()
{
  //Loop through each pin and call the FSR handler
  for(int i=0;i<NUM_PADS;i++)
  {
    handleFSR(i); 
  }
}