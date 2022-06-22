//Arduino for Musicians
//MIDI Hand Drum v6
//Brent Edstrom, 2015


//NOTE: Select appropriate Teensy model from the Tools>Board menu.
//Select MIDI for the USB type

#include <EEPROM.h>
#include <EasyButton.h>
#include <RotaryEncoder.h>
#include "TouchPad.h"

//Button and LED pins:
const int BUTTON_A=1;  //TOP BUTTON 
const int BUTTON_B=0;  //BOTTOM BUTTON
const int LED_A=9;
const int LED_B=10;

EasyButton button_a;
EasyButton button_b;

//Variables:
boolean last_A_status=false;
boolean last_B_status=false;

//Rotary encoder setup:
int status= 0;
int encoderPinA=3;
int encoderPinB=2;

RotaryEncoder encoder;

//Mode of operation constants
const int PLAY_MODE=0;
const int BANK_MODE=1;
const int PROGRAM_MODE=2;

int currentMode=PLAY_MODE;  //this variable indicates the current mode of operation

//Constants for storing data to EPROM
const byte EEPROM_ID=0x99;
const int  ID_ADDRESS=0;
const int  BASE_ADDRESS=1;


//FSR constants:
const int NUM_PADS=6;
const int NO_PAD_SELECTED=-1;
//Top row pin assignments
const int fsrA=17;
const int fsrB=15;
const int fsrC=13;
//Bottom row pin assignments
const int fsrD=16;
const int fsrE=14;
const int fsrF=12;

//Range values
const int minimum=0;
const int maximum=1000;
//Consider adding code for user to adjust the threshold value via the rotary encoder:
int THRESHOLD=100; 

//Variables to track the currently active pad and bank
int current_pad=0;
int current_bank=0;

byte MIDI_NOTES[6][6]={
  42, 46, 44, 35, 38, 49,   /*drums */
  43, 45, 47, 48, 50, 54,   /*toms*/
  62 ,63, 64, 60, 61, 58,   /*conga*/
  65, 66, 67, 68, 69, 70,   /*timbale*/
  73, 73, 75, 76, 77, 78,   /*guiro*/
  37, 39, 51, 52, 57, 55};  /*cymbals*/
  
//Create an array of TouchPad objects
TouchPad TouchPads[NUM_PADS];

void setup() {

  button_a.init(BUTTON_A, 20);
  button_b.init(BUTTON_B, 20);
  encoder.init(encoderPinA, encoderPinB, 20, 24, 0, 127);

  //set pin assignments
  TouchPads[0].setPin(fsrA);
  TouchPads[1].setPin(fsrB);
  TouchPads[2].setPin(fsrC);
  TouchPads[3].setPin(fsrD);
  TouchPads[4].setPin(fsrE);
  TouchPads[5].setPin(fsrF);

  //Set up LEDs
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);

  //Load data from EPROM (this will flash yellow if data found, 
  //red if default values are stored to disk)
  readDataFromEPROM();

  //Delay to stabilize the circuitry
  delay(1000);

}

void loop()
{
    //handle lights and menu buttons
    handleLEDs();
    handleMenuButtons();  

    //Check the current mode status and call the corresponding method
    if(currentMode==PLAY_MODE)
    {
        playMode();
    }
    if(currentMode==BANK_MODE)
    {
        bankMode();
    }
    if(currentMode==PROGRAM_MODE)
    {
        programMode();
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

void bankMode()
{
  //poll FSRs to see if a new bank has been selected
  int val=0;
  int last_bank=current_bank;
  for(int i=0;i<NUM_PADS;i++)
  {
    //Get the analog value of each pad
    val=TouchPads[i].getPadValue(); 

    //Set the index of the active bank if the given pad reading is above the threshold 
    if(val>THRESHOLD)
    {
      current_bank=i;
    }
  }

  //Provide audible feedback indicating the new note selection  
  if(current_bank!=last_bank)
  {
    //Sent a midi note on message followed by a midi note off message
    playMidiAlert(MIDI_NOTES[current_bank][0], 100);
  }

}

void programMode()
{
  //Call play mode so that pads can be played/auditioned during the editing process
  playMode();

  //Check for factory reset
  if(topRowPressed())
  {
    factoryReset();
    flashLED(LED_A,10);     
  }
  
  //poll FSRs to see if a new pad has been selected
  int val=0;
  for(int i=0;i<NUM_PADS;i++)
  {
    //Get the analog value of each pad
    val=TouchPads[i].getPadValue(); 

    //Set the index of the active bank if the given pad reading is above the threshold 
    if(val>THRESHOLD)
    {
      current_pad=i;
      //set the encoder to match this value
      encoder.init(encoderPinA, encoderPinB, 20, MIDI_NOTES[current_bank][current_pad], 0, 127);
    }
  }

  //Get the value of the currently selected bank/pad
  int note=MIDI_NOTES[current_bank][current_pad];
  
  //Poll the encoder
  int encoder_val = encoder.trackShaftPosition();
  if(encoder_val != note)
  {
      MIDI_NOTES[current_bank][current_pad] = encoder_val;
      playMidiAlert(encoder_val, 100);
      delay(300);
  }

}

//Helper method that plays a Note On and Off of the specified pitch and velocity
//Do not use in performance mode due to delay() function
void playMidiAlert(byte note, byte velocity)
{
    //Send midi note on:
    usbMIDI.sendNoteOn(note,velocity,1);
    delay(50);
    //Send midi note off:
    usbMIDI.sendNoteOff(note,0,1);  
}

void handleMenuButtons()
{
  //========Button A: used to switch in and out of bank mode
  boolean a_status=button_a.checkButtonPressed();
  if(a_status !=last_A_status)
  {   
    //Output only on press, not on release
    if(a_status==false)
    {
      if(currentMode==BANK_MODE)
      {
        currentMode=PLAY_MODE;
      } 
      else if(currentMode==PLAY_MODE){
        currentMode=BANK_MODE;
      }

      if(currentMode==PROGRAM_MODE)
      {
        //exiting program mode...save work
        storeDataToEPROM();
        currentMode=BANK_MODE;
      }

    }
    last_A_status=a_status; 
    delay(100); 
  } 

  //=========Button B: used to switch in and out of program mode
  boolean b_status=button_b.checkButtonPressed();
  if(b_status!=last_B_status)
  {   
    //Output only on press, not on release
    if(b_status==false)
    {  
      //Serial.println("Button B pressed");
      if(currentMode==PROGRAM_MODE)
      {
        currentMode=PLAY_MODE;
        storeDataToEPROM(); 
      }
      else{
        currentMode=PROGRAM_MODE; 
      }
    }
    last_B_status=b_status;
    delay(100);
  }  
}

void handleLEDs()
{
  if(currentMode==PLAY_MODE)
  {
    digitalWrite(LED_A, LOW);   //play mode on
    digitalWrite(LED_B, LOW); 
  }
  if(currentMode==BANK_MODE)
  {
    digitalWrite(LED_A, HIGH);  //bank select mode on
    digitalWrite(LED_B, LOW);  
  }
  if(currentMode==PROGRAM_MODE)
  {
    digitalWrite(LED_A, LOW);  
    digitalWrite(LED_B, HIGH);   //program mode on 
  }
}

void storeDataToEPROM()
{
  //store the ID
  EEPROM.write(ID_ADDRESS, EEPROM_ID);
  //loop through note array and store each item to EEPROM
  int address=BASE_ADDRESS;
  for(int bank=0; bank<6 ; bank++)
  {
    for(int note=0;note<6;note++)
    {
      EEPROM.write(address, MIDI_NOTES[bank][note]);
      address++;
    }  
  }

  flashLED(LED_B, 5); 
}

void readDataFromEPROM()
{
  //test for data ID byte:
  byte id=EEPROM.read(ID_ADDRESS);
  if(id==EEPROM_ID)
  {
    //read the data
    //loop through note array and load each item to EEPROM
    int address=BASE_ADDRESS;
    for(int bank=0; bank<6 ; bank++)
    {
      for(int note=0;note<6;note++)
      {
        MIDI_NOTES[bank][note]=EEPROM.read(address);
        address++;
      }  
    }
    flashLED(LED_A, 5); 

  }
  else{
    //No data found: write default data to EEPROM
    storeDataToEPROM();
  }

}

void flashLED(int pin, int num_flashes)
{
  for(int c=0;c<num_flashes;c++)
  {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
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

//Used to trigger a factory reset whilst in Program Mode
boolean topRowPressed()
{
  int val_a = TouchPads[0].getPadValue(); 
  int val_b = TouchPads[1].getPadValue(); 
  int val_c = TouchPads[2].getPadValue(); 
  if(val_a>THRESHOLD && val_b>THRESHOLD && val_c>THRESHOLD)
    return true;

  return false;
}

void factoryReset()
{
  //Re-initialize sounds
  //bank 0
  MIDI_NOTES[0][0]=42;
  MIDI_NOTES[0][1]=46;
  MIDI_NOTES[0][2]=44;
  MIDI_NOTES[0][3]=35;
  MIDI_NOTES[0][4]=38;
  MIDI_NOTES[0][5]=49;

  //bank 1
  MIDI_NOTES[1][0]=43;
  MIDI_NOTES[1][1]=45;
  MIDI_NOTES[1][2]=47;
  MIDI_NOTES[1][3]=38;
  MIDI_NOTES[1][4]=50;
  MIDI_NOTES[1][5]=54;

  //bank 2
  MIDI_NOTES[2][0]=62;
  MIDI_NOTES[2][1]=63;
  MIDI_NOTES[2][2]=64;
  MIDI_NOTES[2][3]=60;
  MIDI_NOTES[2][4]=61;
  MIDI_NOTES[2][5]=58;

  //bank 3
  MIDI_NOTES[3][0]=65;
  MIDI_NOTES[3][1]=66;
  MIDI_NOTES[3][2]=67;
  MIDI_NOTES[3][3]=68;
  MIDI_NOTES[3][4]=69;
  MIDI_NOTES[3][5]=70;

  //bank 4
  MIDI_NOTES[4][0]=73;
  MIDI_NOTES[4][1]=73;
  MIDI_NOTES[4][2]=75;
  MIDI_NOTES[4][3]=76;
  MIDI_NOTES[4][4]=77;
  MIDI_NOTES[4][5]=78;

  //bank 5
  MIDI_NOTES[5][0]=37;
  MIDI_NOTES[5][1]=39;
  MIDI_NOTES[5][2]=51;
  MIDI_NOTES[5][3]=52;
  MIDI_NOTES[5][4]=57;
  MIDI_NOTES[5][5]=55;


}

