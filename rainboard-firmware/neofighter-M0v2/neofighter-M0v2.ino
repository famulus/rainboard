

#include <MIDI.h> 
#include <Wire.h>
#include "Adafruit_MCP23017.h"
extern TwoWire Wire1;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiUSB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midiDIN);

Adafruit_MCP23017 ioExpander; 

const uint8_t number_of_buttons = 61;

const uint8_t modWheel_pin = A9;             
const uint8_t pitchBend_pin = A10;    
const uint8_t softpots_bottom_pin = A11;         

const uint8_t global_midi_channel = 1;
const uint8_t global_midi_velocity = 0x45;

//uint8_t noteRange[200];

uint8_t c2_base_note = 0;

     //Define the pitches, so we don’t have to keep looking up the hex / midi chart                    
uint8_t noteHex[] = 
{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
  0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53,
  0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
  0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};

//const uint8_t  original_button_to_pin[] = 
//{31,35,38,42,46,30,32,36,40,44,51,26,29,33,39,45,48,50,22,25,28,34,41,49,52,A13,17,18,24,27,37,47,A15,A12,A11,14,16,19,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2}; 

const uint8_t  button_to_pin[] = 
{31,35,38,42,46,30,32,36,40,44,3,26,29,33,39,45,48,2,22,25,28,34,41,49,20,A13,0xFF,21,24,27,37,47,A15,A12,0xFF,14,16,17,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2};
//                             *                   *                   *      *    *                      *          * 
// modified pin assignments from V1 to V2 PCB. 0xFF = placeholder; BTN 27 and 35 moved to io expander.

const uint8_t  button_to_wiki[] = 
{42,40,38,36,34,49,47,45,43,41,39,56,54,52,50,48,46,44,63,61,59,57,55,53,51,49,70,68,66,64,62,60,58,56,54,75,73,71,69,67,65,63,61,80,78,76,74,72,70,68,85,83,81,79,77,75,90,88,86,84,82};                  

 
//scanModWheel()
uint8_t ModWheelCurrentPosition;
uint8_t ModWheelPreviousPosition;
uint64_t LastModWheelEventTime;

//scanPitchBend()
uint8_t PitchBendCurrentPosition;
uint8_t PitchBendPreviousPosition;
uint8_t PitchBendValue;
uint64_t LastPitchBendEventTime;

//scanButtons()
boolean ButtonState[number_of_buttons];
boolean ReadyForPress[number_of_buttons];
boolean ReadyForRelease[number_of_buttons];
boolean NoteOnDeadZone[number_of_buttons];
boolean NoteOffDeadZone[number_of_buttons];
uint64_t LastNoteOnEventTime[number_of_buttons];
uint64_t LastNoteOffEventTime[number_of_buttons];

boolean NoteOnFlag[number_of_buttons];
uint8_t ButtonIdleCount[number_of_buttons];

// configure timing
const uint64_t dead_zone_time = 2;
const uint8_t button_idle_period_cycles = 100;

const uint8_t modWheel_scan_period = 10;  
const uint8_t pitchBend_scan_period = 1;  

// configure limits
const uint8_t modWheel_ceiling = 20;
const uint8_t modWheel_floor = 20;
const uint8_t pitchBend_ceiling = 20;
const uint8_t pitchBend_floor = 20;
const uint8_t pitchBend_dead_center = 20;
const uint16_t softpots_max = 1023; 

////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){  
                                                              
  pinMode(modWheel_pin, INPUT);                                // init softpots
  ModWheelCurrentPosition = ModWheelPreviousPosition = 0;    
  pinMode(pitchBend_pin, INPUT);  
  PitchBendCurrentPosition = PitchBendPreviousPosition = 0;
  PitchBendValue = 0;
  pinMode(softpots_bottom_pin, INPUT);
  analogReference(EXTERNAL);
  
  LastModWheelEventTime = LastPitchBendEventTime = millis();  // init timekeepers  

  initIoExpander();                         // init io expander
  initButtons();                            // init buttons  
  
  midiUSB.begin(MIDI_CHANNEL_OMNI);         // init midi ports
  midiUSB.turnThruOff();
  midiDIN.begin(MIDI_CHANNEL_OMNI);
  midiDIN.turnThruOn();  

  
    
                     
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  scanButtonsV1();
  
  scanModWheel();
  
  //scanPitchBendStandard(); 

  
  //midiNoteOn(60, 100);
  //delay(1000);
  //midiNoteOff(60);
  //delay(1000);

  //for(uint8_t i=0; i<16; i++){ 
  //   if(ioExpander.digitalRead(i) == 0){
  //    midiNoteOn(60, 100);   
  //    delay(100);  
  //   }
  //}

}


/////////////////////////////////////// SCAN MOD AND PITCH///////////////////////////////////////

void scanPitchBendStandard() {

  static uint16_t pitch_bend_raw;
  static uint16_t softpots_bottom_raw;
  static uint16_t pitch_bend_min;
  static uint16_t pitch_bend_max;
  static uint16_t pitch_bend_center;
  static uint16_t pitch_bend_up_min;
  static uint16_t pitch_bend_down_max;

  if(millis() < LastPitchBendEventTime){                                           // check for rollover
    return;
  }

  if(millis() > LastPitchBendEventTime + pitchBend_scan_period){                    // throttle and debounce
                                          
    softpots_bottom_raw = analogRead(softpots_bottom_pin);                          // get softpots bottom voltage
    pitch_bend_raw = analogRead(pitchBend_pin);                                     // get pitch bend voltage

    pitch_bend_min = softpots_bottom_raw + pitchBend_floor;                             // set lower limit
    pitch_bend_max = softpots_max - pitchBend_ceiling;                                  // set upper limit
    pitch_bend_center = (pitch_bend_min + ((pitch_bend_max - pitch_bend_min) / 2));     // set center             
    pitch_bend_up_min = (pitch_bend_center + (pitchBend_dead_center / 2));              // set pitch bend up lower limit
    pitch_bend_down_max = (pitch_bend_center - (pitchBend_dead_center / 2));            // set pitch bend down upper limit   
    
    if(pitch_bend_raw > softpots_bottom_raw){                                                             // if its not a release

      if((pitch_bend_raw >= pitch_bend_min) && (pitch_bend_raw <= pitch_bend_down_max)){                  // if it is a pitch down
          PitchBendCurrentPosition = map(pitch_bend_raw, pitch_bend_min, pitch_bend_down_max, 0, 63);     // get pitch down value            
      }

      else if((pitch_bend_raw >= pitch_bend_up_min) && (pitch_bend_raw <= pitch_bend_max)){               // if it is a pitch up
          PitchBendCurrentPosition = map(pitch_bend_raw, pitch_bend_up_min, pitch_bend_max, 64, 127);     // get pitch up value          
      }
             
      if((PitchBendCurrentPosition >= 0) && (PitchBendCurrentPosition <= 127)){                 // if in range
         if(PitchBendCurrentPosition != PitchBendPreviousPosition){                             // and has changed
            midiPitchBend(PitchBendCurrentPosition);                                            // send midi            
            PitchBendPreviousPosition = PitchBendCurrentPosition;                    // update pitch bend value
            LastPitchBendEventTime = millis();                                       // update timekeeper
         } 
      }
    }      
  }   
}


void scanModWheel() {

  static uint16_t mod_wheel_raw;
  static uint16_t softpots_bottom_raw;
  static uint16_t mod_wheel_min;
  static uint16_t mod_wheel_max;

  if(millis() < LastModWheelEventTime){                                           // check for rollover
    return;
  }

  if(millis() > LastModWheelEventTime + modWheel_scan_period){                    // throttle and debounce
                                          
    softpots_bottom_raw = analogRead(softpots_bottom_pin);                        // get softpots bottom voltage
    mod_wheel_raw = analogRead(modWheel_pin);                                     // get mod wheel voltage

    mod_wheel_min = softpots_bottom_raw + modWheel_floor;                         // set lower limit
    mod_wheel_max = softpots_max - modWheel_ceiling;                              // set upper limit
    
    if(mod_wheel_raw > softpots_bottom_raw){                                                   // if its not a release
       ModWheelCurrentPosition = map(mod_wheel_raw, mod_wheel_min, mod_wheel_max, 0, 127);     // get mod wheel value
       if((ModWheelCurrentPosition >= 0) && (ModWheelCurrentPosition <= 127)){                 // if in range
          if(ModWheelCurrentPosition != ModWheelPreviousPosition){                             // and has changed
             midiModWheel(ModWheelCurrentPosition);                                           // send midi
             ModWheelPreviousPosition = ModWheelCurrentPosition;                    // update mod wheel value
             LastModWheelEventTime = millis();                                      // update timekeeper
          } 
       }
    }      
  }   
}


///////////////////////////////////////// SCAN BUTTONS V1 /////////////////////////////////////////

void scanButtonsV1(){   // adjustable dead zone. read then process. kill stuck notes

  acquireButtons();
    
  Serial.flush();

  for(uint8_t i=0; i<number_of_buttons; i++){      

      if(!ButtonState[i]){                                                                 // if button is low
          if(ReadyForPress[i]){                                                            // and ready for press                                
              midiNoteOn(noteHex[button_to_wiki[i]+c2_base_note], global_midi_velocity);   // initiate button press event
              NoteOnFlag[i] = true;
              LastNoteOnEventTime[i] = millis();                                           // store time
              ReadyForPress[i] = false;                                                    // press has been processed
              NoteOnDeadZone[i] = true;                                                    // now in noteOn dead zone              
          }
          else if(NoteOnDeadZone[i]){                                              // if in noteOn dead zone
              if(millis() >= (LastNoteOnEventTime[i] + dead_zone_time)){           // and noteOn dead zone is over
                  ReadyForRelease[i] = true;                                       // ready for release
                  NoteOnDeadZone[i] = false;                                       // out of noteOn dead zone
              }
          }
      }
          
      else {                                                                              // if button is high
          if(ReadyForRelease[i]){                                                         // and ready for release                
              midiNoteOff(noteHex[button_to_wiki[i]+c2_base_note]);                       // initiate button release event
              NoteOnFlag[i] = false;
              LastNoteOffEventTime[i] = millis();                                         // store time
              ReadyForRelease[i] = false;                                                 // release has been processed
              NoteOffDeadZone[i] = true;                                                  // now in noteOff dead zone              
          }  
          else if(NoteOffDeadZone[i]){                                             // if in noteOff dead zone
              if(millis() >= (LastNoteOffEventTime[i] + dead_zone_time)){          // and noteOff dead zone is over
                  ReadyForPress[i] = true;                                         // ready for press
                  NoteOffDeadZone[i] = false;                                      // out of noteOff dead zone
              }
          }

          ButtonIdleCount[i]++;
          if(ButtonIdleCount[i] >= button_idle_period_cycles){
              if(NoteOnFlag[i]){
                  midiNoteOff(noteHex[button_to_wiki[i]+c2_base_note]); 
                  NoteOnFlag[i] = false;   
              }
              ButtonIdleCount[i] = 0;
          }
      }
  }     
}

///////////////////////////////////////// MIDI MESSAGES //////////////////////////////////////

void midiNoteOn(uint8_t pitch, uint8_t velocity){
  
     midiUSB.sendNoteOn(pitch, velocity, global_midi_channel);
     midiDIN.sendNoteOn(pitch, velocity, global_midi_channel);
}

void midiNoteOff(uint8_t pitch){
      
     midiNoteOn(pitch, 0);    
}

void midiCCout(uint8_t ccNum, uint8_t ccVal){
  
    midiUSB.sendControlChange(ccNum, ccVal, global_midi_channel);
    midiDIN.sendControlChange(ccNum, ccVal, global_midi_channel);
}

void midiModWheel(uint8_t modVal){  
    
    midiCCout(0x01, modVal);    
}

void midiPitchBend(uint8_t pitchVal){ 
  
    //uint8_t pitchLSB = 0;
    //if(pitchVal == 127){
    //  pitchLSB = 127;  
    //}

    //uint16_t pitchFullVal = ((pitchVal << 8) + pitchLSB);
    
    //Serial.write(midi_pitchBend_cmd | (global_midi_channel - 1));
    //Serial.write(pitchLSB);
    //Serial.write(pitchVal);

    midiUSB.sendPitchBend(pitchVal, global_midi_channel);
    midiDIN.sendPitchBend(pitchVal, global_midi_channel);
}

/////////////////////////////////////// ACQUIRE BUTTONS ////////////////////////////////////

void acquireButtons(){   

    uint8_t portdata;
    portdata = PINA;
    ButtonState[18] = bitRead(portdata,0);
    ButtonState[38] = bitRead(portdata,1);
    ButtonState[28] = bitRead(portdata,2);
    ButtonState[19] = bitRead(portdata,3);
    ButtonState[11] = bitRead(portdata,4);
    ButtonState[29] = bitRead(portdata,5);
    ButtonState[20] = bitRead(portdata,6);
    ButtonState[12] = bitRead(portdata,7);
    portdata = PINB;    
    //ButtonState[24] = bitRead(portdata,1);    // button 25 old
    //ButtonState[10] = bitRead(portdata,2);    // button 11 old
    //ButtonState[17] = bitRead(portdata,3);    // button 18 old
    ButtonState[51] = bitRead(portdata,4);
    ButtonState[50] = bitRead(portdata,5);
    ButtonState[56] = bitRead(portdata,6);
    ButtonState[57] = bitRead(portdata,7);
    portdata = PINC;
    ButtonState[30] = bitRead(portdata,0);
    ButtonState[7]  = bitRead(portdata,1);
    ButtonState[1]  = bitRead(portdata,2);
    ButtonState[21] = bitRead(portdata,3);
    ButtonState[13] = bitRead(portdata,4);
    ButtonState[6]  = bitRead(portdata,5);
    ButtonState[0]  = bitRead(portdata,6);
    ButtonState[5]  = bitRead(portdata,7);
    portdata = PIND;     
    //ButtonState[37] = bitRead(portdata,2);    // button 38 old
    //ButtonState[27] = bitRead(portdata,3);    // button 28 old 
    ButtonState[27] = bitRead(portdata,0);  // new button 28
    ButtonState[24] = bitRead(portdata,1);  // new button 25
    ButtonState[2]  = bitRead(portdata,7);
    portdata = PINE; 
    ButtonState[43] = bitRead(portdata,3);
    ButtonState[17] = bitRead(portdata,4);  // new button 18
    ButtonState[10] = bitRead(portdata,5);  // new button 11
    portdata = PINF;
    ButtonState[59] = bitRead(portdata,0);
    ButtonState[54] = bitRead(portdata,1);
    ButtonState[60] = bitRead(portdata,2);
    ButtonState[55] = bitRead(portdata,3);
    ButtonState[47] = bitRead(portdata,4);
    ButtonState[49] = bitRead(portdata,5);
    ButtonState[48] = bitRead(portdata,6);
    ButtonState[42] = bitRead(portdata,7);
    portdata = PING;
    ButtonState[22] = bitRead(portdata,0);
    ButtonState[8]  = bitRead(portdata,1);
    ButtonState[14] = bitRead(portdata,2);
    ButtonState[44] = bitRead(portdata,5);
    portdata = PINH;
    //ButtonState[26] = bitRead(portdata,0);    // button 27 old
    ButtonState[37] = bitRead(portdata,0);  // new button 38
    ButtonState[36] = bitRead(portdata,1);    
    ButtonState[46] = bitRead(portdata,3);
    ButtonState[53] = bitRead(portdata,4);
    ButtonState[58] = bitRead(portdata,5);
    ButtonState[52] = bitRead(portdata,6);
    portdata = PINJ;
    ButtonState[45] = bitRead(portdata,0);
    ButtonState[35] = bitRead(portdata,1); 
    portdata = PINK;
    ButtonState[41] = bitRead(portdata,0);
    //ButtonState[34] = bitRead(portdata,3);    // button 35 old
    ButtonState[33] = bitRead(portdata,4);
    ButtonState[25] = bitRead(portdata,5);
    ButtonState[40] = bitRead(portdata,6);
    ButtonState[32] = bitRead(portdata,7);   
    portdata = PINL;
    ButtonState[23] = bitRead(portdata,0);
    ButtonState[16] = bitRead(portdata,1);
    ButtonState[31] = bitRead(portdata,2);
    ButtonState[4]  = bitRead(portdata,3);
    ButtonState[15] = bitRead(portdata,4);
    ButtonState[9]  = bitRead(portdata,5);
    ButtonState[39] = bitRead(portdata,6);
    ButtonState[3]  = bitRead(portdata,7); 

    ButtonState[34] = ioExpander.digitalRead(4);  // new button 35
    ButtonState[26] = ioExpander.digitalRead(5);  // new button 27
           
}

void acquireButtonsOriginal(){    // 1.68 us execution time  

    uint8_t portdata;
    portdata = PINA;
    ButtonState[18] = bitRead(portdata,0);
    ButtonState[38] = bitRead(portdata,1);
    ButtonState[28] = bitRead(portdata,2);
    ButtonState[19] = bitRead(portdata,3);
    ButtonState[11] = bitRead(portdata,4);
    ButtonState[29] = bitRead(portdata,5);
    ButtonState[20] = bitRead(portdata,6);
    ButtonState[12] = bitRead(portdata,7);
    portdata = PINB;    
    ButtonState[24] = bitRead(portdata,1);
    ButtonState[10] = bitRead(portdata,2);
    ButtonState[17] = bitRead(portdata,3);
    ButtonState[51] = bitRead(portdata,4);
    ButtonState[50] = bitRead(portdata,5);
    ButtonState[56] = bitRead(portdata,6);
    ButtonState[57] = bitRead(portdata,7);
    portdata = PINC;
    ButtonState[30] = bitRead(portdata,0);
    ButtonState[7]  = bitRead(portdata,1);
    ButtonState[1]  = bitRead(portdata,2);
    ButtonState[21] = bitRead(portdata,3);
    ButtonState[13] = bitRead(portdata,4);
    ButtonState[6]  = bitRead(portdata,5);
    ButtonState[0]  = bitRead(portdata,6);
    ButtonState[5]  = bitRead(portdata,7);
    portdata = PIND;     
    ButtonState[37] = bitRead(portdata,2);
    ButtonState[27] = bitRead(portdata,3);   
    ButtonState[2]  = bitRead(portdata,7);
    portdata = PINE; 
    ButtonState[43] = bitRead(portdata,3);
    portdata = PINF;
    ButtonState[59] = bitRead(portdata,0);
    ButtonState[54] = bitRead(portdata,1);
    ButtonState[60] = bitRead(portdata,2);
    ButtonState[55] = bitRead(portdata,3);
    ButtonState[47] = bitRead(portdata,4);
    ButtonState[49] = bitRead(portdata,5);
    ButtonState[48] = bitRead(portdata,6);
    ButtonState[42] = bitRead(portdata,7);
    portdata = PING;
    ButtonState[22] = bitRead(portdata,0);
    ButtonState[8]  = bitRead(portdata,1);
    ButtonState[14] = bitRead(portdata,2);
    ButtonState[44] = bitRead(portdata,5);
    portdata = PINH;
    ButtonState[26] = bitRead(portdata,0);
    ButtonState[36] = bitRead(portdata,1);    
    ButtonState[46] = bitRead(portdata,3);
    ButtonState[53] = bitRead(portdata,4);
    ButtonState[58] = bitRead(portdata,5);
    ButtonState[52] = bitRead(portdata,6);
    portdata = PINJ;
    ButtonState[45] = bitRead(portdata,0);
    ButtonState[35] = bitRead(portdata,1); 
    portdata = PINK;
    ButtonState[41] = bitRead(portdata,0);
    ButtonState[34] = bitRead(portdata,3);
    ButtonState[33] = bitRead(portdata,4);
    ButtonState[25] = bitRead(portdata,5);
    ButtonState[40] = bitRead(portdata,6);
    ButtonState[32] = bitRead(portdata,7);   
    portdata = PINL;
    ButtonState[23] = bitRead(portdata,0);
    ButtonState[16] = bitRead(portdata,1);
    ButtonState[31] = bitRead(portdata,2);
    ButtonState[4]  = bitRead(portdata,3);
    ButtonState[15] = bitRead(portdata,4);
    ButtonState[9]  = bitRead(portdata,5);
    ButtonState[39] = bitRead(portdata,6);
    ButtonState[3]  = bitRead(portdata,7); 
        
}

/////////////////////////////////////// INIT BUTTONS ////////////////////////////////////

void initButtons(){  

  for(uint8_t i=0; i<number_of_buttons; i++){      // init BTNs loop    

    if( (i != 26) && (i != 34)){                  // skip BTN 27 and 35
      pinMode(button_to_pin[i], INPUT_PULLUP);    // inputs w/ pullups     
    }
  
    ButtonState[i] = 1;                            // debounce flags
    ReadyForPress[i] = 1;
    ReadyForRelease[i] = 0;
    NoteOnDeadZone[i] = 0;
    NoteOffDeadZone[i] = 0;
    NoteOnFlag[i] = 0;
    ButtonIdleCount[i] = 0;   

    //noteRange[i] = noteHex[i];
    
  }   
}

void initIoExpander(){
  
  ioExpander.begin();
  for(uint8_t i=0; i<16; i++){ 
    ioExpander.pinMode(i, INPUT);
    ioExpander.pullUp(i, HIGH);    
  }
}
