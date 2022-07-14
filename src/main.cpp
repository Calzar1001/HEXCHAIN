/**
 * HEXCHAIN
 * V1
 *  Touchpad Midi controller
 * */

#include <Arduino.h>
#include <TouchPad.h>

// Touch Pads constants:
const int NUM_PADS = 6;
// Touch pad pin assignments
const int fsrA = 17;
const int fsrB = 15;
const int fsrC = 13;
const int fsrD = 16;
const int fsrE = 14;
const int fsrF = 12;

// Range values
const int minimum = 0;
const int maximum = 1000;
// Threshhold value for note on
int THRESHOLD = 100;

byte MIDI_NOTES[6][6] = {
    42, 46, 44, 35, 38, 49,  /*drums */
    43, 45, 47, 48, 50, 54,  /*toms*/
    62, 63, 64, 60, 61, 58,  /*conga*/
    65, 66, 67, 68, 69, 70,  /*timbale*/
    73, 73, 75, 76, 77, 78,  /*guiro*/
    37, 39, 51, 52, 57, 55}; /*cymbals*/

// Create an array of TouchPad objects
TouchPad TouchPads[NUM_PADS];

void setup()
{
  // set pin assignments
  TouchPads[0].setPin(fsrA);
  TouchPads[1].setPin(fsrB);
  TouchPads[2].setPin(fsrC);
  TouchPads[3].setPin(fsrD);
  TouchPads[4].setPin(fsrE);
  TouchPads[5].setPin(fsrF);

  // Delay to stabilize the circuitry
  delay(1000);
}

void loop()
{
  // Loop through each pin and call the FSR handler
  for (int i = 0; i < NUM_PADS; i++)
  {
    handleFSR(i);
  }
}

void handleFSR(int index)
{
  // read FSR values, scale to midi velocity (0-127) and output as MIDI notes
  int val = TouchPads[index].getPadValue();
  int last_value = 0;

  // Scale the FSR value to a valid MIDI velocity range
  int midi_velocity = map(val, minimum, maximum, 0, 127);

  //===========Send a NOTE ON message if the given pad is currently off
  if (val > THRESHOLD && TouchPads[index].getNoteOnStatus() == false)
  {

    TouchPads[index].setNoteOnStatus(true);

    // send a single note on for value greater than 0
    usbMIDI.sendNoteOn(MIDI_NOTES[current_bank][index], midi_velocity, 1);
  }

  //===========Send a NOTE OFF message if the given pad is currently on
  if (midi_velocity == 0 && TouchPads[index].getNoteOnStatus() == true)
  {
    // send a single note off for velocity of 0
    usbMIDI.sendNoteOff(MIDI_NOTES[current_bank][index], 0, 1);
    TouchPads[index].setNoteOnStatus(false);
  }
}