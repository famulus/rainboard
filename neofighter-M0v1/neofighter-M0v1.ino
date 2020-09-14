 

const uint8_t midi_noteOn_command = 0x90;
const uint8_t midi_cc_command = 0xB0;
const uint8_t midi_pitchBend_cmd = 0xE0;
const uint8_t midi_modWheel_number = 0x01;

const uint8_t number_of_buttons = 61;

const uint8_t modWheel_pin = A9;             
const uint8_t pitchBend_pin = A10;            

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

const uint8_t  button_to_pin[] = 
{31,35,38,42,46,30,32,36,40,44,51,26,29,33,39,45,48,50,22,25,28,34,41,49,52,A13,17,18,24,27,37,47,A15,A12,A11,14,16,19,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2}; 

const uint8_t  button_to_wiki[] = 
{42,40,38,36,34,49,47,45,43,41,39,56,54,52,50,48,46,44,63,61,59,57,55,53,51,49,70,68,66,64,62,60,58,56,54,75,73,71,69,67,65,63,61,80,78,76,74,72,70,68,85,83,81,79,77,75,90,88,86,84,82};                  

//scanButtonsOriginal()
boolean changeState[number_of_buttons] = {false};
boolean currentState[number_of_buttons] = {false};
 

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

const uint64_t dead_zone_time = 2;

boolean NoteOnFlag[number_of_buttons];
uint8_t ButtonIdleCount[number_of_buttons];
const uint8_t button_idle_period_cycles = 100;

const uint8_t modWheel_scan_period = 10;  
const uint8_t pitchBend_scan_period = 1;  

////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
                                         // init softpots
  pinMode(modWheel_pin, INPUT);
  ModWheelCurrentPosition = ModWheelPreviousPosition = map(analogRead(modWheel_pin), 0, 1023, 0, 128);    
  pinMode(pitchBend_pin, INPUT);  
  PitchBendCurrentPosition = PitchBendPreviousPosition = map(analogRead(pitchBend_pin), 0, 1023, 0, 128);
  LastModWheelEventTime = LastPitchBendEventTime = millis();
   
  for(uint8_t i=0; i<number_of_buttons; i++){      //Initialization loop
  
    pinMode(button_to_pin[i], INPUT_PULLUP);      //Initialize pins as inputs w/ pullups

    ButtonState[i] = 1;                 // init debounce flags
    ReadyForPress[i] = 1;
    ReadyForRelease[i] = 0;
    NoteOnDeadZone[i] = 0;
    NoteOffDeadZone[i] = 0;
    NoteOnFlag[i] = 0;
    ButtonIdleCount[i] = 0;

    //noteRange[i] = noteHex[i];
    
  }    
  Serial.begin(31250);                 // init midi port    
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  scanButtonsV4();

  //scanButtonsV3();
  
  //scanButtonsV2();
  
  //scanButtonsV1();
  
  scanModWheel();
  
  scanPitchBend();

}

///////////////////////////////////////// SCAN BUTTONS V4 /////////////////////////////////////////

void scanButtonsV4(){   // adjustable dead zone. read then process. kill stuck notes

  acquireButtonsFast();
  //acquireButtonsSlow();
   
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

///////////////////////////////////////// SCAN BUTTONS V3 /////////////////////////////////////////

void scanButtonsV3(){   // adjustable dead zone. read then process

  acquireButtonsFast();
  //acquireButtonsSlow();
   
  Serial.flush();

  for(uint8_t i=0; i<number_of_buttons; i++){      

      if(!ButtonState[i]){                                                                 // if button is low
          if(ReadyForPress[i]){                                                            // and ready for press                          
              midiNoteOn(noteHex[button_to_wiki[i]+c2_base_note], global_midi_velocity);   // initiate button press event
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
      }
  }     
}

///////////////////////////////////////// SCAN BUTTONS V2 /////////////////////////////////////////

void scanButtonsV2(){    // fixed dead zone. continuously process
   
  Serial.flush();

  for(uint8_t i=0; i<number_of_buttons; i++){ 

      ButtonState[i] = (digitalRead(button_to_pin[i]));                                   // read button

      if(!ButtonState[i]){                                                                // if button is low
          if(ReadyForPress[i]){                                                           // and ready for press                          
              midiNoteOn(noteHex[button_to_wiki[i]+c2_base_note], global_midi_velocity);  // initiate button press event
              LastNoteOnEventTime[i] = millis();                                          // store time
              ReadyForPress[i] = false;                                                   // press has been processed
              NoteOnDeadZone[i] = true;                                                   // now in noteOn dead zone
          }
          else if(NoteOnDeadZone[i]){                                // if in noteOn dead zone
              if(LastNoteOnEventTime[i] != millis()){                // and noteOn dead zone is over
                  ReadyForRelease[i] = true;                         // ready for release
                  NoteOnDeadZone[i] = false;                         // out of noteOn dead zone
              }
          }
      }
          
      else {                                                                              // if button is high
          if(ReadyForRelease[i]){                                                         // and ready for release                
              midiNoteOff(noteHex[button_to_wiki[i]+c2_base_note]);                       // initiate button release event
              LastNoteOffEventTime[i] = millis();                                         // store time
              ReadyForRelease[i] = false;                                                 // release has been processed
              NoteOffDeadZone[i] = true;                                                  // now in noteOff dead zone              
          }  
          else if(NoteOffDeadZone[i]){                              // if in noteOff dead zone
              if(LastNoteOffEventTime[i] != millis()){              // and noteOff dead zone is over
                  ReadyForPress[i] = true;                          // ready for press
                  NoteOffDeadZone[i] = false;                       // out of noteOff dead zone
              }
          }
      }
  }     
}

/////////////////////////////////////// SCAN MOD AND PITCH///////////////////////////////////////

void scanModWheel() {

  if(millis() < LastModWheelEventTime){                                           // check for rollover
    return;
  }

  if(millis() > LastModWheelEventTime + modWheel_scan_period){                    // throttle and debounce
    ModWheelCurrentPosition = map(analogRead(modWheel_pin), 0, 1023, 0, 128);     // get mod wheel value
    if(ModWheelCurrentPosition > 0){                                              // if its not a release
       if(ModWheelCurrentPosition != ModWheelPreviousPosition){                   // and has changed
           midiModWheel(ModWheelCurrentPosition - 1);                             // send midi
           ModWheelPreviousPosition = ModWheelCurrentPosition;                    // update mod wheel value
           LastModWheelEventTime = millis();                                      // update timekeeper
       }
    }      
  }   
}

void scanPitchBend() {
  
  if(millis() < LastPitchBendEventTime){                                            // check for rollover
    return;
  }

  if(millis() > LastPitchBendEventTime + pitchBend_scan_period){                    // throttle and debounce
    PitchBendCurrentPosition = map(analogRead(pitchBend_pin), 0, 1023, 0, 128);     // get pitchbend value
    if(PitchBendCurrentPosition > 0){                                               // if its not a release
       if(PitchBendCurrentPosition != PitchBendPreviousPosition){                   // and has changed
           midiPitchBend(PitchBendCurrentPosition - 1);                             // send midi
           PitchBendPreviousPosition = PitchBendCurrentPosition;                    // update pitchbend value
           LastPitchBendEventTime = millis();                                       // update timekeeper
       }
    }  
    else{                                                                           // if release
      if(PitchBendPreviousPosition){
          midiPitchBend(64);                                                        // send pitchbend center  
          PitchBendPreviousPosition = 0;                                            // update pitchbend value
      }     
    }
  }  
}

///////////////////////////////////////// MIDI MESSAGES //////////////////////////////////////

void midiNoteOn(uint8_t pitch, uint8_t velocity){
  
     Serial.write(midi_noteOn_command | (global_midi_channel - 1));
     Serial.write(pitch);
     Serial.write(velocity);    
}

void midiNoteOff(uint8_t pitch){
      
     midiNoteOn(pitch, 0);    
}

void midiCCout(uint8_t ccNum, uint8_t ccVal){
  
    Serial.write(midi_cc_command | (global_midi_channel - 1));
    Serial.write(ccNum);
    Serial.write(ccVal);
}

void midiModWheel(uint8_t modVal){  
    
    midiCCout(midi_modWheel_number, modVal);    
}

void midiPitchBend(uint16_t pitchVal){ 
  
    uint8_t pitchLSB = 0;
    if(pitchVal == 127){
      pitchLSB = 127;  
    }
    Serial.write(midi_pitchBend_cmd | (global_midi_channel - 1));
    Serial.write(pitchLSB);
    Serial.write(pitchVal);
}

/////////////////////////////////////// ACQUIRE BUTTONS ////////////////////////////////////

void acquireButtonsSlow(){    // 208 us execution time
  
    for(uint8_t i=0; i<number_of_buttons; i++){ 
      ButtonState[i] = (digitalRead(button_to_pin[i]));       
   } 
}

void acquireButtonsFast(){    // 1.68 us execution time  

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

///////////////////////////////////////// SCAN BUTTONS V1 /////////////////////////////////////////
void scanButtonsV1() {           // original code
    
  for(uint8_t i=0; i<number_of_buttons; i++){      //Read all of the buttons, then store into state bool vars
    
      if(digitalRead(button_to_pin[i]) == LOW){
        changeState[i] = true;
      }
      else{
        changeState[i] = false;
      }
  } 
   
  Serial.flush();

  for(uint8_t i=0; i<number_of_buttons; i++){ 
     
    if(changeState[i] == true){      //If the button is pressed
        
      if(currentState[i] != changeState[i]){   //If this is the first reading after pressing
              
         if(i == 26){                    // these two buttons serve as half tone up or down.
           c2_base_note = (c2_base_note + 1);
         }
         else if(i == 34){
           c2_base_note = (c2_base_note - 1);
         }
        
         else {
            midiNoteOn(noteHex[button_to_wiki[i]+c2_base_note], global_midi_velocity);
         }
         
      }
    }
    else{      //If the button is not pressed
      if(currentState[i] != changeState[i]){   //if this is the first reading after releasing
        
           midiNoteOff(noteHex[button_to_wiki[i]+c2_base_note]);
      }
    }     
    currentState[i] = changeState[i] ;  //Keep a record of the previous state, so we can monitor button state changes
  } 
}

// original code below

/*

//Allocate the buttons to the digi IOs on the board, pins 2-7
#define PITCH_1 0x3F 



//For now, all midi commands are on channel 1
#define CMD_1 0x90
#define CMD_2 0x90
#define CMD_3 0x90
#define CMD_4 0x90
#define CMD_5 0x90
#define CMD_6 0x90

//With medium velocity
#define VEL_1 0x64
#define VEL_2 0x45
#define VEL_3 0x45
#define VEL_4 0x45
#define VEL_5 0x45
#define VEL_6 0x45

#define DEBUG false



//Define the pitches, so we don’t have to keep looking
// up the hex / midi chart

byte noteHex[] = 
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
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
};




static const uint8_t  button_to_pin[] = 
{
31,35,38,42,46,30,32,36,40,44,51,26,29,33,39,45,48,50,22,25,28,34,41,49,52,A13,17,18,24,27,37,47,A15,A12,A11,14,16,19,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2
}; 

static const uint8_t  button_to_wiki[] = 
{
42,40,38,36,34,49,47,45,43,41,39,56,54,52,50,48,46,44,63,61,59,57,55,53,51,49,70,68,66,64,62,60,58,56,54,75,73,71,69,67,65,63,61,80,78,76,74,72,70,68,85,83,81,79,77,75,90,88,86,84,82,
};                  

boolean  changeState[61];
boolean currentState[61];

byte noteRange[200];

int c2_base_note = 0;


void setup()
{
    //Set up the digi IOs as inputs for the buttons
  for(int i=0; i<61; i++)
  {
    pinMode(button_to_pin[i], INPUT_PULLUP);
    currentState[i] = false;
    changeState[i] = false;
    noteRange[i] = noteHex[i];

  }
  // Set MIDI baud rate:
  if(DEBUG){
    Serial.begin(9600);
  }else{
    Serial.begin(31250);
  }
}




void loop()
{

  // delay(500);
  //Read all of the buttons, then store into state bool vars
  for(int i=0; i<61; i++)
  {

    if(digitalRead(button_to_pin[i]) == LOW) {changeState[i] = true;}
    else {changeState[i] = false;}
  } 
  // end loop

  //Flush the serial buffer
  Serial.flush();

  for(int i=0; i<61; i++) // for each button
  {
    //If the button is pressed
    if(changeState[i] == true) 
    {
    //If this is the first reading after pressing
      if(currentState[i] != changeState[i]){

        if(DEBUG){
          Serial.print( i );
          Serial.print( ":" );
          Serial.println( button_to_wiki[i] );
        }


        // these two notes serve as half tone up or down.
        if(i == 26){
          c2_base_note = c2_base_note + 1;
        }
        else if (i == 34){
          c2_base_note = c2_base_note - 1;
        }
        else{
        //send command to start corresponding note
        writeNote(CMD_1, noteHex[button_to_wiki[i]+c2_base_note], VEL_1);

        }



      }
    }
    //If the button is not pressed
    else
    {
    //if this is the first reading after releasing
      if(currentState[i] != changeState[i]){
      //send the command to stop corresponding note
        stopNote(CMD_1, noteHex[button_to_wiki[i]+c2_base_note]);
      }
    }

    //Keep a record of the previous state, so we can monitor
    //button state changes
    currentState[i] = changeState[i] ;
  } // end each note


}// end loop


void writeNote(byte command, byte pitch, byte velocity)
{


  // Serial.print( command );
  // Serial.print("  " );
  // Serial.println(  pitch  );
  // Serial.print("  " );
  // Serial.println(  velocity);

  if(DEBUG){}else{
     Serial.write(command);
     Serial.write(pitch);
     Serial.write(velocity);
  }



}

void stopNote(byte command, byte pitch)
{
//No way to know (without keeping track of which pitch was
// played for each button) which pitch to turn off,
//So switch them both off

  if(DEBUG){}else{
   Serial.write(command);
   Serial.write(pitch);
   Serial.write(0x00);
  }


}

*/
