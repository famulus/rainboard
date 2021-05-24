

#include <MIDI.h>                 // https://github.com/FortySevenEffects/arduino_midi_library

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiUSB);

const uint8_t number_of_buttons = 61;   

uint8_t global_midi_channel = 1;
uint8_t global_midi_velocity = 102;

// default notemap_array
uint8_t  button_to_wiki[] = 
{42,40,38,36,34,49,47,45,43,41,39,56,54,52,50,48,46,44,63,61,59,57,55,53,51,49,70,68,66,64,62,60,58,56,54,75,73,71,69,67,65,63,61,80,78,76,74,72,70,68,85,83,81,79,77,75,90,88,86,84,82};

// note assignments to btns. loaded with loadNoteMap(notemap_array)                 
uint8_t current_note_map[number_of_buttons] = {0}; 

//scanButtons 
const uint8_t READY_FOR_PRESS = 1;
const uint8_t WAIT_FOR_IDLE_LO = 2;
const uint8_t READY_FOR_RELEASE = 3;
const uint8_t WAIT_FOR_IDLE_HI = 4;
const uint8_t HI = 1;
const uint8_t LO = 0;

uint8_t ButtonPhase[number_of_buttons];
boolean ButtonState[number_of_buttons];     
uint8_t LastPitchSent[number_of_buttons];  
boolean ButtonActive[number_of_buttons]; 

uint16_t ButtonIdleCountLo[number_of_buttons];
uint16_t ButtonIdleCountHi[number_of_buttons];
uint64_t LastLoDetectedTime[number_of_buttons];
uint64_t LastHiDetectedTime[number_of_buttons];
uint16_t ButtonWatchdogCount[number_of_buttons];

const uint64_t idle_time_lo = 200;                // in microseconds 
const uint64_t idle_time_hi = 200;                // in microseconds 

const uint16_t button_watchdog_timeout = 5;       // iterations of button scan


////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){  
 
  initButtons();                            // init buttons  
    
  midiUSB.begin(MIDI_CHANNEL_OMNI);         // init midi ports
  midiUSB.turnThruOff();

  loadNoteMap(button_to_wiki);              // load default note map           
                       
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
   
  scanButtons();
  
}

///////////////////////////////////////// SCAN BUTTONS ////////////////////////////////////////////
void scanButtons(){  
  
  uint64_t Now;

  acquireButtons();     // gets all 61 BTNs and stores in ButtonState[] array. 
     
  Serial.flush();

    for(uint8_t i=0; i<number_of_buttons; i++){   

      if(ButtonState[i] == LO){ 
          ButtonWatchdogCount[i] = 0;     
      }
      if(ButtonState[i] == HI){ 
          ButtonWatchdogCount[i]++;     
      }

      if( (ButtonActive[i] == true) && (ButtonWatchdogCount[i] >= button_watchdog_timeout)){
          buttonReleaseEvent(i);                           
          ButtonActive[i] = false;
          ButtonWatchdogCount[i] = 0;
          ButtonPhase[i] = READY_FOR_PRESS;                       
      }

      if(ButtonPhase[i] == READY_FOR_PRESS){             // if ready for press
          if(ButtonState[i] == LO){                      // and LO detected then it is the first falling edge
              buttonPressEvent(i);                       // initiate button press event
              ButtonActive[i] = true;
              ButtonPhase[i] = WAIT_FOR_IDLE_LO;         // next button phase
              LastLoDetectedTime[i] = micros();          // mark the time of first falling edge
              ButtonIdleCountLo[i] = 0;                  // reset idle LO counter
          }
      }
      
      else if(ButtonPhase[i] == WAIT_FOR_IDLE_LO){       // if waiting for idle LO
          Now = micros();                          // * Deal with Overflow
          if(ButtonState[i] == LO){                              // if it is LO
              if(ButtonIdleCountLo[i] >= idle_time_lo){              // and idle LO counter has expired
                  ButtonPhase[i] = READY_FOR_RELEASE;                // next button phase                  
              }
              else if(Now > LastLoDetectedTime[i]){                  // or if time has passed               
                  ButtonIdleCountLo[i] = ButtonIdleCountLo[i] + (Now - LastLoDetectedTime[i]);  // update idle LO counter   
                  LastLoDetectedTime[i] = Now;                       // mark the time of last LO detected
              }                 
          }
          else if(ButtonState[i] == HI){                        // if it is HI
              ButtonIdleCountLo[i] = 0;                         // then its a bounce, reset idle LO counter
          }         
      }
      
      else if(ButtonPhase[i] == READY_FOR_RELEASE){      // if ready for release
          if(ButtonState[i] == HI){                             // and HI detected then it is the first rising edge
              ButtonPhase[i] = WAIT_FOR_IDLE_HI;                // next button phase 
              LastHiDetectedTime[i] = micros();                 // mark the time of first rising edge
              ButtonIdleCountHi[i] = 0;                         // reset idle HI counter     
          }       
      }
      
      else if(ButtonPhase[i] == WAIT_FOR_IDLE_HI){       // if waiting for idle HI
          Now = micros();                          // * Deal with Overflow
          if(ButtonState[i] == LO){                             // if it is LO
              ButtonIdleCountHi[i] = 0;                         // then its a bounce, reset idle HI counter
          }
          else if(ButtonState[i] == HI){                        // if it is HI
              if(ButtonIdleCountHi[i] >= idle_time_hi){             // and idle HI counter has expired
                  buttonReleaseEvent(i);                            // initiate button release event
                  ButtonActive[i] = false;
                  ButtonPhase[i] = READY_FOR_PRESS;                 // next button phase                  
              }
              else if(Now > LastHiDetectedTime[i]){                 // or if time has passed                
                  ButtonIdleCountHi[i] = ButtonIdleCountHi[i] + (Now - LastHiDetectedTime[i]);  // update idle HI counter 
                  LastHiDetectedTime[i] = Now;                      // mark the time of last HI detected
              }         
          }    
      }
  }   
    
}

void buttonPressEvent(uint8_t button){
  
    uint8_t midi_note;
    midi_note = current_note_map[button];                  // lookup pitch 
    midiNoteOn(midi_note, global_midi_velocity);           // send note on
    LastPitchSent[button] = midi_note;                     // store pitch sent
    
}

void buttonReleaseEvent(uint8_t button){

    midiNoteOff(LastPitchSent[button]);                    // send note off
    
}

////////////////////////////////////////// LOAD NOTE MAP ////////////////////////////////////////
void loadNoteMap(uint8_t *note_map){

    for(uint8_t i=0; i<number_of_buttons; i++){
      current_note_map[i] = *note_map; 
      note_map++; 
    }  
  
}


///////////////////////////////////////// MIDI MESSAGES //////////////////////////////////////
void midiNoteOn(uint8_t pitch, uint8_t velocity){
  
     midiUSB.sendNoteOn(pitch, velocity, global_midi_channel);
     
}

void midiNoteOff(uint8_t pitch){
      
     midiNoteOn(pitch, 0); 
        
}


/////////////////////////////////////// ACQUIRE BUTTONS ////////////////////////////////////
void acquireButtons(){   

    uint8_t portdata;
    portdata = PINA;
    ButtonState[14] = bitRead(portdata,0);
    ButtonState[34] = bitRead(portdata,1);
    ButtonState[19] = bitRead(portdata,2);
    ButtonState[15] = bitRead(portdata,3);
    ButtonState[8]  = bitRead(portdata,4);
    ButtonState[18] = bitRead(portdata,5);
    ButtonState[16] = bitRead(portdata,6);
    ButtonState[7]  = bitRead(portdata,7);
    portdata = PINB;
    ButtonState[50] = bitRead(portdata,0);    
    ButtonState[47] = bitRead(portdata,4);
    ButtonState[46] = bitRead(portdata,5);
    ButtonState[53] = bitRead(portdata,6);
    ButtonState[52] = bitRead(portdata,7);
    portdata = PINC;
    ButtonState[26] = bitRead(portdata,0);
    ButtonState[3]  = bitRead(portdata,1);   
    ButtonState[17] = bitRead(portdata,3);
    ButtonState[6]  = bitRead(portdata,4);
    ButtonState[2]  = bitRead(portdata,5);   
    ButtonState[1]  = bitRead(portdata,7);
    portdata = PINE;
    ButtonState[38] = bitRead(portdata,3);
    ButtonState[13] = bitRead(portdata,4);
    ButtonState[9]  = bitRead(portdata,5);
    ButtonState[57] = bitRead(portdata,6);
    ButtonState[58] = bitRead(portdata,7);
    portdata = PINF;
    ButtonState[55] = bitRead(portdata,1);
    ButtonState[56] = bitRead(portdata,2);
    ButtonState[54] = bitRead(portdata,3);
    ButtonState[43] = bitRead(portdata,4);
    ButtonState[45] = bitRead(portdata,5);
    ButtonState[44] = bitRead(portdata,6);
    ButtonState[39] = bitRead(portdata,7);
    portdata = PING;
    ButtonState[25] = bitRead(portdata,0);
    ButtonState[4]  = bitRead(portdata,1);
    ButtonState[5]  = bitRead(portdata,2);
    ButtonState[59] = bitRead(portdata,3);
    ButtonState[60] = bitRead(portdata,4);
    ButtonState[37] = bitRead(portdata,5);
    portdata = PINH;
    ButtonState[33] = bitRead(portdata,0);  
    ButtonState[35] = bitRead(portdata,3);
    ButtonState[49] = bitRead(portdata,4);
    ButtonState[48] = bitRead(portdata,5);
    ButtonState[51] = bitRead(portdata,6);
    ButtonState[32] = bitRead(portdata,7);
    portdata = PINJ;
    ButtonState[36] = bitRead(portdata,0);
    ButtonState[31] = bitRead(portdata,1);
    ButtonState[23] = bitRead(portdata,2);
    ButtonState[20] = bitRead(portdata,3);
    ButtonState[21] = bitRead(portdata,4);
    ButtonState[30] = bitRead(portdata,5);
    portdata = PINK;
    ButtonState[40] = bitRead(portdata,0);
    ButtonState[29] = bitRead(portdata,4);
    ButtonState[22] = bitRead(portdata,5);
    ButtonState[41] = bitRead(portdata,6);
    ButtonState[28] = bitRead(portdata,7);
    portdata = PINL;
    ButtonState[24] = bitRead(portdata,0);
    ButtonState[12] = bitRead(portdata,1);
    ButtonState[27] = bitRead(portdata,2);
    ButtonState[0]  = bitRead(portdata,3);
    ButtonState[11] = bitRead(portdata,4);
    ButtonState[10] = bitRead(portdata,5);
    ButtonState[42] = bitRead(portdata,6);
               
}


/////////////////////////////////////// INIT HARDWARE ////////////////////////////////////
void initButtons(){  

  DDRA = B00000000;             // set all BTNs to inputs
  DDRB = DDRB & B00001110;     
  DDRC = DDRC & B01000100;
  DDRE = DDRE & B00000111;
  DDRF = DDRF & B00000001;
  DDRG = B000000;
  DDRH = DDRH & B00000110;
  DDRJ = DDRJ & B11000000;
  DDRK = DDRK & B00001110;
  DDRL = DDRL & B10000000;

  PORTA = B11111111;            // set all BTNs to have pullups
  PORTB = PORTB | B11110001;      
  PORTC = PORTC | B10111011;
  PORTE = PORTE | B11111000;
  PORTF = PORTF | B11111110;
  PORTG = B111111;
  PORTH = PORTH | B11111001;
  PORTJ = PORTJ | B00111111;
  PORTK = PORTK | B11110001;
  PORTL = PORTL | B01111111;

  for(uint8_t i=0; i<number_of_buttons; i++){     // init button debounce  
  
    ButtonState[i] = HI;                          
    ButtonPhase[i] = READY_FOR_PRESS;
    ButtonActive[i] = false;
    ButtonWatchdogCount[i] = 0;
    
  }   
  
}
