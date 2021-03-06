

#include <MIDI.h> 
#include <Wire.h>
#include "Adafruit_MCP23017.h"
extern TwoWire Wire1;
// #define TWI_FREQ 400000L in Arduino\hardware\arduino\avr\libraries\Wire\src\utility\twi.h for 400kHz i2c mode

Adafruit_MCP23017 ioExpander;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiUSB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midiDIN); 

const uint8_t number_of_buttons = 61;
const uint8_t number_of_meta_buttons = 16; // only 12 actual meta buttons (extra 4 are overlap of BTNs 27,25,28, & 35 on IO expander)

const uint8_t modWheel_pin = A9;             
const uint8_t pitchBend_pin = A10;    
const uint8_t softpots_bottom_pin = A11;         

const uint8_t midi_note_max = 127;
const uint8_t midi_note_min = 0;
const uint8_t midi_velocity_max = 127;
const uint8_t midi_velocity_min = 2;
const uint8_t midi_velocity_increment = 25;

uint8_t global_midi_channel = 1;
uint8_t global_midi_velocity = 102;


const uint8_t  button_to_pin[] = 
{31,35,38,42,46,30,32,36,40,44,3,26,29,33,39,45,48,2,22,25,28,34,41,49,0xFF,A13,0xFF,0xFF,24,27,37,47,A15,A12,0xFF,14,16,17,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2};
//                             *                   *                   *        *    *                        *          * 
// modified pin assignments from V1 to V3 PCB. 0xFF = placeholder; BTN 27, 35, 25, and 28 moved to io expander.


uint8_t  button_to_wiki[] = 
{42,40,38,36,34,49,47,45,43,41,39,56,54,52,50,48,46,44,63,61,59,57,55,53,51,49,70,68,66,64,62,60,58,56,54,75,73,71,69,67,65,63,61,80,78,76,74,72,70,68,85,83,81,79,77,75,90,88,86,84,82};
                  
uint8_t current_note_map[number_of_buttons] = {0}; 

const uint8_t UP = 1;
const uint8_t DOWN = 0;
const uint8_t ON = 127;
const uint8_t OFF = 0;

 
//scanModWheel()
uint8_t ModWheelCurrentPosition;
uint8_t ModWheelPreviousPosition;
uint64_t LastModWheelEventTime;

//scanPitchBend()
uint8_t PitchBendCurrentPosition;
uint8_t PitchBendPreviousPosition;
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
uint8_t LastPitchSent[number_of_buttons];

//scanMetaButtons()
boolean MetaButtonState[number_of_meta_buttons];
boolean MetaButtonReadyForPress[number_of_meta_buttons];
boolean MetaButtonReadyForRelease[number_of_meta_buttons];
boolean MetaButtonPressDeadZone[number_of_meta_buttons];
boolean MetaButtonReleaseDeadZone[number_of_meta_buttons];
uint64_t LastMetaButtonPressTime[number_of_meta_buttons];
uint64_t LastMetaButtonReleaseTime[number_of_meta_buttons];

boolean MetaButtonPressedFlag[number_of_meta_buttons];
uint8_t MetaButtonIdleCount[number_of_meta_buttons];

uint8_t ControllerNumber;
boolean ControllerActive = false;

// configure timing
const uint64_t dead_zone_time = 2;                   // in milliseconds
const uint8_t button_idle_period_cycles = 5;         // iterations of scanButtons() @1.52ms (@880us in 400kHz i2c mode)
const uint64_t meta_button_dead_zone_time = 2;       // in milliseconds
const uint8_t meta_button_idle_period_cycles = 5;    // iterations of scanMetaButtons() @4.56ms (@2.08ms in 400kHz i2c mode)

const uint8_t modWheel_scan_period = 10;             // in milliseconds  
const uint8_t pitchBend_scan_period = 1;             // in milliseconds  

// configure limits
const uint8_t modWheel_ceiling = 20;                 // in samples (total of 1024)
const uint8_t modWheel_floor = 20;
const uint8_t pitchBend_ceiling = 20;
const uint8_t pitchBend_floor = 20;
const uint8_t pitchBend_dead_center = 20;
const uint16_t softpots_max = 1023; 

// configure modes
const boolean PitchInverted = false;
const boolean ModInverted = false;
const boolean IgnoreChannelShift = true;

////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){  

  initIoExpander();                         // init io expander
  initButtons();                            // init buttons  
  initSoftpots();                           // init softpots   
  
  midiUSB.begin(MIDI_CHANNEL_OMNI);         // init midi ports
  midiUSB.turnThruOff();
  midiDIN.begin(MIDI_CHANNEL_OMNI);
  midiDIN.turnThruOn();   

  loadNoteMap(button_to_wiki);
                       
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() { 
  
  scanMetaButtons();
  
  scanButtons();
  
  scanModWheel();
  
  scanPitchBendStandard(); 

  if (midiDIN.read()) {      // MIDI DIN Input echo thru to MIDI USB
     midiUSB.send(midiDIN.getType(), midiDIN.getData1(), midiDIN.getData2(), midiDIN.getChannel());
  }
   
 
}


////////////////////////////////////////// LOAD NOTE MAP ////////////////////////////////////////
void loadNoteMap(uint8_t *note_map){

  for(uint8_t i=0; i<number_of_buttons; i++){
    current_note_map[i] = *note_map; 
    note_map++; 
  }  
}

/////////////////////////////////////////// SCAN PITCH//////////////////////////////////////////
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
         if(PitchInverted){
          PitchBendCurrentPosition = map(PitchBendCurrentPosition, 0, 127, 127, 0);  
         }
         if(PitchBendCurrentPosition != PitchBendPreviousPosition){                             // and has changed
            midiPitchBend(PitchBendCurrentPosition);                                            // send midi            
            PitchBendPreviousPosition = PitchBendCurrentPosition;                               // update pitch bend value
            LastPitchBendEventTime = millis();                                                  // update timekeeper
         } 
      }
    } 
    else if(pitch_bend_raw < softpots_bottom_raw){                    // if it is a release  
      if(PitchBendCurrentPosition != 64){                             // and pitch bend not centered
        midiPitchBend(64);                                            // send midi pitch bend center 
        PitchBendPreviousPosition = PitchBendCurrentPosition = 64;    // update pitch bend value
        LastPitchBendEventTime = millis();                            // update timekeeper
      }          
    }
  }   
}

/////////////////////////////////////////// SCAN MOD  ///////////////////////////////////////////
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
          if(ModInverted){
             ModWheelCurrentPosition = map(ModWheelCurrentPosition, 0, 127, 127, 0);  
          }
          if(ModWheelCurrentPosition != ModWheelPreviousPosition){                             // and has changed
             if(ControllerActive == false){
                midiModWheel(ModWheelCurrentPosition);                                         // send mod
             }
             if(ControllerActive == true){
                midiCCout(ControllerNumber, ModWheelCurrentPosition);                          // send cc
             }
                                                      
             
             ModWheelPreviousPosition = ModWheelCurrentPosition;                               // update mod wheel value
             LastModWheelEventTime = millis();                                                 // update timekeeper
          } 
       }
    }      
  }   
}

///////////////////////////////////////// SCAN BUTTONS ////////////////////////////////////////////
void scanButtons(){  

  uint8_t midi_note;

  acquireButtons();     // gets all 61 BTNs and stores in ButtonState[] array. including BTN 27, 35, 25, and 28 on the IO expander
    
  Serial.flush();

  for(uint8_t i=0; i<number_of_buttons; i++){      

      if(!ButtonState[i]){                                                         // if button is low
          if(ReadyForPress[i]){                                                            // and ready for press                                         
              midi_note = current_note_map[i];                                             // lookup pitch 
              midiNoteOn(midi_note, global_midi_velocity);                                 // initiate button press event
              LastPitchSent[i] = midi_note;
              NoteOnFlag[i] = true;
              LastNoteOnEventTime[i] = millis();                                           // store time
              ReadyForPress[i] = false;                                                    // press has been processed
              NoteOnDeadZone[i] = true;                                                    // now in noteOn dead zone              
          }
          else if(NoteOnDeadZone[i]){                                                      // if in noteOn dead zone
              if(millis() >= (LastNoteOnEventTime[i] + dead_zone_time)){                   // and noteOn dead zone is over
                  ReadyForRelease[i] = true;                                               // ready for release
                  NoteOnDeadZone[i] = false;                                               // out of noteOn dead zone
              }
          }
      }
          
      else {                                                                       // if button is high
          if(ReadyForRelease[i]){                                                         // and ready for release                
              midiNoteOff(LastPitchSent[i]);                                              // initiate button release event
              NoteOnFlag[i] = false;
              LastNoteOffEventTime[i] = millis();                                         // store time
              ReadyForRelease[i] = false;                                                 // release has been processed
              NoteOffDeadZone[i] = true;                                                  // now in noteOff dead zone              
          }  
          else if(NoteOffDeadZone[i]){                                                    // if in noteOff dead zone
              if(millis() >= (LastNoteOffEventTime[i] + dead_zone_time)){                 // and noteOff dead zone is over
                  ReadyForPress[i] = true;                                                // ready for press
                  NoteOffDeadZone[i] = false;                                             // out of noteOff dead zone
              }
          }

          ButtonIdleCount[i]++;                                                           // kill stuck notes  
          if(ButtonIdleCount[i] >= button_idle_period_cycles){
              if(NoteOnFlag[i]){                                                                      // * BTN 35, 27, 25, and 28 -> GPIOA.4, GPIOA.5, GPIOA.7, GPIOA.6
                  midiNoteOff(LastPitchSent[i]); 
                  NoteOnFlag[i] = false;   
              }
              ButtonIdleCount[i] = 0;
          }
      }
  }     
}

/////////////////////////////////////////  META BUTTON FUNCTIONS /////////////////////////////////////////

// * pitch shifting functions assume BTN1 is lowest pich and BTN61 is highest pitch in notemap
void octaveShift(uint8_t shift_direction){

  if(shift_direction == UP){
    if((current_note_map[60] + 12) < midi_note_max){
      for(uint8_t i=0; i<number_of_buttons; i++){  
        current_note_map[i] = (current_note_map[i]+12);   
      }
    }
  }  
  if(shift_direction == DOWN){
    if((current_note_map[0] - 12) > midi_note_min){
      for(uint8_t i=0; i<number_of_buttons; i++){  
        current_note_map[i] = (current_note_map[i]-12);      
      }
    }  
  }
}

void semitoneShift(uint8_t shift_direction){

  if(shift_direction == UP){
    if((current_note_map[60] + 1) < midi_note_max){
      for(uint8_t i=0; i<number_of_buttons; i++){  
        current_note_map[i]++;   
      }
    }
  }  
  if(shift_direction == DOWN){
    if((current_note_map[0] - 1) > midi_note_min){
      for(uint8_t i=0; i<number_of_buttons; i++){  
        current_note_map[i]--;   
      }
    }  
  }  
}

void velocityShift(uint8_t shift_direction){

  if(shift_direction == UP){
    if(global_midi_velocity <= (midi_velocity_max - midi_velocity_increment)){     
         global_midi_velocity = (global_midi_velocity + midi_velocity_increment);    
    }    
  }  
  if(shift_direction == DOWN){
    if(global_midi_velocity >= (midi_velocity_min + midi_velocity_increment)){   
         global_midi_velocity = (global_midi_velocity - midi_velocity_increment);   
    }    
  }  
}

void channelShift(uint8_t shift_direction){
  if(IgnoreChannelShift == true){
    return;
  }
  if(shift_direction == UP){
     if(global_midi_channel < 16){       
       global_midi_channel++;
     }     
  }  
  if(shift_direction == DOWN){
    if(global_midi_channel > 1){        
       global_midi_channel--;
    }     
  }  
}

void zeroAllControllers(){

}

/////////////////////////////////////// DIGIT ENTRY ////////////////////////////////////
uint16_t acquireDigitStates(){   
                                
    uint16_t DigitButtonState = 0;     // active high logic                                
    uint8_t portdata;
    
    if(!ioExpander.digitalRead(5)){ bitSet(DigitButtonState,9); }   // button 27  //// 9     
    if(!ioExpander.digitalRead(6)){ bitSet(DigitButtonState,8); }   // button 28  //// 8    
    portdata = PINA;   
    if(!bitRead(portdata,2)){ bitSet(DigitButtonState,7); }         // button 29  //// 7    
    if(!bitRead(portdata,5)){ bitSet(DigitButtonState,6); }         // button 30  //// 6      
    portdata = PINC;
    if(!bitRead(portdata,0)){ bitSet(DigitButtonState,5); }         // button 31  //// 5        
    portdata = PINL;  
    if(!bitRead(portdata,2)){ bitSet(DigitButtonState,4); }         // button 32  //// 4       
    portdata = PINK;   
    if(!bitRead(portdata,7)){ bitSet(DigitButtonState,3); }         // button 33  //// 3       
    if(!bitRead(portdata,4)){ bitSet(DigitButtonState,2); }         // button 34  //// 2       
    if(!ioExpander.digitalRead(4)){ bitSet(DigitButtonState,1); }   // button 35  //// 1    
    if(!bitRead(portdata,5)){ bitSet(DigitButtonState,0); }         // button 26  //// 0    
       
    return(DigitButtonState);
}

uint8_t getDigit(){

    uint16_t digitStates = 0;
    uint16_t digitMask;
 
    while(!digitStates){
      digitStates = acquireDigitStates();  
    }

    while(digitStates != 1 && digitStates != 2 && digitStates != 4 && digitStates != 8 && digitStates != 16 && digitStates != 32 && digitStates != 64 && digitStates != 128 && digitStates != 256 && digitStates != 512){
      digitStates = acquireDigitStates();  
    };
    
    for(uint8_t digit=0; digit<=9; digit++){
       digitMask = 1 << digit;
       if(digitStates == digitMask){
          do{ digitStates = acquireDigitStates(); }
          while(digitStates);
          return(digit);  
       }      
    }     
}

uint8_t getThreeDigitNumber(){

  uint8_t digit = 0;
  uint8_t ThreeDigits = 0;

  digit = getDigit();
  ThreeDigits = (digit * 100);
  digit = getDigit();
  ThreeDigits = ThreeDigits + (digit * 10);
  digit = getDigit();
  ThreeDigits = ThreeDigits + digit;
  if((ThreeDigits >= 0) && (ThreeDigits <= 127)){
    return(ThreeDigits);  
  }
  else return(0xFF);  
  
}

/////////////////////////////////////////  META BUTTONS HANDLER /////////////////////////////////////////
void metaButtonHandler(uint8_t meta_button){ 

  uint8_t ThreeDigitsEntered;

  switch (meta_button) {

    case 0: // GPIOA.0 J13
      velocityShift(DOWN);     // Velocity - 
      break;
    case 1: // GPIOA.1 J14
      velocityShift(UP);       // Velocity + 
      break;
    case 2: // GPIOA.2 J15
      octaveShift(DOWN);       // Octave - 
      break;
    case 3: // GPIOA.3 J16
      octaveShift(UP);         // Octave +  
      break;
      
    //// * case 4-7 overlap onto BTNs: // GPIOA.4 used for BTN35 // GPIOA.5 used for BTN27 // GPIOA.6 J17 used for BTN28 // GPIOA.7 J18 used for BTN25
    
    case 8: // GPIOB.0 J1
      semitoneShift(UP);       // Semitone +     
      break;
    case 9: // GPIOB.1 J2
      semitoneShift(DOWN);     // Semitone -
      break;
    case 10: // GPIOB.2 J3 
      channelShift(UP);        // Channel +
      break;
    case 11: // GPIOB.3 J4
      channelShift(DOWN);      // Channel -
      break;
    case 12:  // GPIOB.4 J5
      ControllerNumber = getThreeDigitNumber();
      ControllerActive = true;
      midiCCout(ControllerNumber, 0);
      break;
    case 13: // GPIOB.5 J6
      ControllerActive = false;
      break;    
    case 14: // GPIOB.6 J7    
      zeroAllControllers();
      break;
    case 15: // GPIOB.7 J8
      ThreeDigitsEntered = getThreeDigitNumber();
      midiProgramChange(ThreeDigitsEntered);
      break;
    
    default:
      break;
  }
  
}

/////////////////////////////////////////// SCAN META BUTTONS //////////////////////////////////////////
void scanMetaButtons(){   

  for(uint8_t i=0; i<number_of_meta_buttons; i++){             // gets all 16 pins of the IO expander which includes 12 meta buttons 
      MetaButtonState[i] = ioExpander.digitalRead(i);          // and also overlap of BTN 35, 27, 25, and 28 onto GPIOA.4, GPIOA.5, GPIOA.7, GPIOA.6
  }                                                           

  for(uint8_t i=0; i<number_of_meta_buttons; i++){      

      if(!MetaButtonState[i]){                                                      // if button is low
          if(MetaButtonReadyForPress[i]){                                                    // and ready for press                                
              metaButtonHandler(i);                                                          // initiate meta button press event
              MetaButtonPressedFlag[i] = true;
              LastMetaButtonPressTime[i] = millis();                                         // store time
              MetaButtonReadyForPress[i] = false;                                            // press has been processed
              MetaButtonPressDeadZone[i] = true;                                             // now in meta button press dead zone              
          }
          else if(MetaButtonPressDeadZone[i]){                                               // if in meta button press dead zone
              if(millis() >= (LastMetaButtonPressTime[i] + meta_button_dead_zone_time)){     // and meta button press dead zone is over
                  MetaButtonReadyForRelease[i] = true;                                       // ready for release
                  MetaButtonPressDeadZone[i] = false;                                        // out of meta button press dead zone
              }
          }
      }
          
      else {                                                                          // if button is high
          if(MetaButtonReadyForRelease[i]){                                                  // and ready for release                
              ////                                                                           // do nothing but housekeeping on meta button release event
              MetaButtonPressedFlag[i] = false;
              LastMetaButtonReleaseTime[i] = millis();                                       // store time
              MetaButtonReadyForRelease[i] = false;                                          // release has been processed
              MetaButtonReleaseDeadZone[i] = true;                                           // now in meta button release dead zone              
          }  
          else if(MetaButtonReleaseDeadZone[i]){                                             // if in meta button release dead zone
              if(millis() >= (LastMetaButtonReleaseTime[i] + meta_button_dead_zone_time)){   // and meta button release dead zone is over
                  MetaButtonReadyForPress[i] = true;                                         // ready for press
                  MetaButtonReleaseDeadZone[i] = false;                                      // out of meta button release dead zone
              }
          }
          
          MetaButtonIdleCount[i]++;
          if(MetaButtonIdleCount[i] >= meta_button_idle_period_cycles){
              if(MetaButtonPressedFlag[i]){
                  ////                                                                       // do nothing but housekeeping on forced meta button release event
                  MetaButtonPressedFlag[i] = false;   
              }
              MetaButtonIdleCount[i] = 0;
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

void midiProgramChange(uint8_t programNumber){
  
    midiUSB.sendProgramChange(programNumber, global_midi_channel);
    midiDIN.sendProgramChange(programNumber, global_midi_channel);
}

void midiModWheel(uint8_t modVal){  
    
    midiCCout(0x01, modVal);    
}

void midiPitchBend(uint8_t pitchVal){ 

   int16_t pitchFullVal;
   
    pitchFullVal = map(pitchVal, 0, 127, -8192, 8191); 

    //midiUSB.sendPitchBend(pitchFullVal, global_midi_channel);
    //midiDIN.sendPitchBend(pitchFullVal, global_midi_channel);
    
    if(pitchVal == 127){
      midiUSB.send(midi::PitchBend, 127, 127, global_midi_channel);  
      midiDIN.send(midi::PitchBend, 127, 127, global_midi_channel);  
    }
    else {
      midiUSB.send(midi::PitchBend, 0, pitchFullVal, global_midi_channel);
      midiDIN.send(midi::PitchBend, 0, pitchFullVal, global_midi_channel);
    }
    
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
    ButtonState[2]  = bitRead(portdata,7);
    portdata = PINE; 
    ButtonState[43] = bitRead(portdata,3);
    ButtonState[17] = bitRead(portdata,4); 
    ButtonState[10] = bitRead(portdata,5); 
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
    ButtonState[37] = bitRead(portdata,0); 
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

    ButtonState[34] = ioExpander.digitalRead(4);  // button 35
    ButtonState[26] = ioExpander.digitalRead(5);  // button 27
    ButtonState[27] = ioExpander.digitalRead(6);  // button 28
    ButtonState[24] = ioExpander.digitalRead(7);  // button 25
           
}


/////////////////////////////////////// INIT HARDWARE ////////////////////////////////////
void initButtons(){  

  for(uint8_t i=0; i<number_of_buttons; i++){     // init BTNs loop    

    if( (i != 26) && (i != 34) && (i != 24) && (i != 27)){    // skip BTN 27 ,35, 25, 28
      pinMode(button_to_pin[i], INPUT_PULLUP);    // inputs w/ pullups     
    }
  
    ButtonState[i] = 1;                           // button debounce flags
    ReadyForPress[i] = 1;
    ReadyForRelease[i] = 0;
    NoteOnDeadZone[i] = 0;
    NoteOffDeadZone[i] = 0;
    NoteOnFlag[i] = 0;
    ButtonIdleCount[i] = 0;   
    
  }   
}

void initIoExpander(){
  
  ioExpander.begin();
  for(uint8_t i=0; i<number_of_meta_buttons; i++){    // init Meta BTNs loop (and also BTN 27 ,35, 25, and 28)
    ioExpander.pinMode(i, INPUT);                     // all pins inputs w/ pullups
    ioExpander.pullUp(i, HIGH);    

    MetaButtonState[i] = 1;                 // meta button debounce flags
    MetaButtonReadyForPress[i] = 1;
    MetaButtonReadyForRelease[i] = 0;
    MetaButtonPressDeadZone[i] = 0;
    MetaButtonReleaseDeadZone[i] = 0;
    MetaButtonPressedFlag[i] = 0;
    MetaButtonIdleCount[i] = 0;   
  }
}

void initSoftpots(){  

   pinMode(modWheel_pin, INPUT);                                // init softpots
   pinMode(pitchBend_pin, INPUT);  
   pinMode(softpots_bottom_pin, INPUT);
   analogReference(EXTERNAL);

   ModWheelCurrentPosition = ModWheelPreviousPosition = 0;     // init controllers
   PitchBendCurrentPosition = PitchBendPreviousPosition = 0; 
   LastModWheelEventTime = LastPitchBendEventTime = millis();  // init timekeepers  
  
}
