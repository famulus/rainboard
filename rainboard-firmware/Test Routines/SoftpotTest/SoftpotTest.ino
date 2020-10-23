 

const uint8_t midi_cc_command = 0xB0;
const uint8_t midi_pitchBend_cmd = 0xE0;
const uint8_t midi_modWheel_number = 0x01;

const uint8_t number_of_buttons = 61;

const uint8_t modWheel_pin = A9;             
const uint8_t pitchBend_pin = A10;  
const uint8_t softpot_feedback_pin = A10;           

const uint8_t global_midi_channel = 1;


//scanModWheel()
uint8_t ModWheelCurrentPosition;
uint8_t ModWheelPreviousPosition;
uint64_t LastModWheelEventTime;

const uint8_t mod_wheel_dead_top = 10;
const uint8_t mod_wheel_dead_bottom = 10;
const uint8_t mod_wheel_span = (mod_wheel_dead_bottom + 127 + mod_wheel_dead_top);                


//scanPitchBend()
uint8_t PitchBendCurrentPosition;
uint8_t PitchBendPreviousPosition;
uint8_t PitchBendValue;
uint64_t LastPitchBendEventTime;

const uint8_t pitch_bend_dead_top = 1;
const uint8_t pitch_bend_dead_bottom = 1;
const uint8_t pitch_bend_dead_center = 1;
const uint8_t pitch_bend_span = (128 + pitch_bend_dead_bottom + pitch_bend_dead_center + pitch_bend_dead_top);      //   (0 is release. 1 - 128 is 0 - 127)
const uint8_t pitch_bend_0 = (pitch_bend_dead_bottom + 1);                                                           
const uint8_t pitch_bend_63 = (pitch_bend_0 + 64); 
const uint8_t pitch_bend_64 = (pitch_bend_63 + pitch_bend_dead_center + 1); 
const uint8_t pitch_bend_127 = (pitch_bend_64 + 64); 

const uint64_t dead_zone_time = 2;


const uint8_t modWheel_scan_period = 10;  
const uint8_t pitchBend_scan_period = 1;  

////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
                                         // init softpots
  pinMode(modWheel_pin, INPUT);
  ModWheelCurrentPosition = ModWheelPreviousPosition = map(analogRead(modWheel_pin), 0, 1023, 0, mod_wheel_span);    
  pinMode(pitchBend_pin, INPUT);  
  PitchBendCurrentPosition = PitchBendPreviousPosition = map(analogRead(pitchBend_pin), 0, 1023, 0, pitch_bend_span);
  
  LastModWheelEventTime = LastPitchBendEventTime = millis(); // init timekeepers
  PitchBendValue = 0;

  pinMode(softpot_feedback_pin, INPUT);
  analogReference(EXTERNAL);
     
  
  Serial.begin(9600);                 // init debug port    
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {


  checkSoftpots(); 

}


void checkSoftpots() {

  uint16_t rawval;

  delay(100);

  rawval = map(analogRead(A9), 0, 1023, 0, 1023);
  Serial.print("A9 MOD = ");
  Serial.print(rawval);

  rawval = map(analogRead(A10), 0, 1023, 0, 1023);
  Serial.print("   A10 PITCH = ");
  Serial.print(rawval);

  rawval = map(analogRead(A11), 0, 1023, 0, 1023);
  Serial.print("   A11 BOTTOM = ");
  Serial.println(rawval);
  
}


/////////////////////////////////////// SCAN MOD AND PITCH///////////////////////////////////////

void scanPitchBendV2() {

  uint8_t pitchBend;
  
  if(millis() < LastPitchBendEventTime){                                            // check for rollover
    return;
  }

  if(millis() > LastPitchBendEventTime + pitchBend_scan_period){                                                   // throttle and debounce
    PitchBendCurrentPosition = map(analogRead(pitchBend_pin), 0, 1023, 0, pitch_bend_span);                        // get pitchbend position
    if(PitchBendCurrentPosition != PitchBendPreviousPosition) {                                                    // if it has changed
        if(PitchBendCurrentPosition == 0){                                                                         // if release
            if(PitchBendValue != 0){                                                                               // and last event was in active range
                midiPitchBend(64);                                                                                 // send pitchbend center  
                PitchBendPreviousPosition = 0;                                                                     // update pitchbend position
                LastPitchBendEventTime = millis();                                                                 // update timekeeper
                PitchBendValue = 0;                                                                                // update pitchbend value
                return;                                                                                            // exit
            }           
        }
        if((PitchBendCurrentPosition >= pitch_bend_0) && (PitchBendCurrentPosition <= pitch_bend_63)){             // if pitch down
             pitchBend = (PitchBendCurrentPosition - pitch_bend_dead_bottom - 1);
             midiPitchBend(pitchBend);                                                                             // send midi
             PitchBendPreviousPosition = PitchBendCurrentPosition;                                                 // update pitchbend position
             LastPitchBendEventTime = millis();                                                                    // update timekeeper
             PitchBendValue = pitchBend;                                                                           // update pitchbend value
             return;                                                                                               // exit
        }
        if((PitchBendCurrentPosition >= pitch_bend_64) && (PitchBendCurrentPosition <= pitch_bend_127)){           // if pitch up
             pitchBend = (PitchBendCurrentPosition + pitch_bend_dead_bottom + 1 + pitch_bend_dead_center);
             midiPitchBend(pitchBend);                                                                             // send midi
             PitchBendPreviousPosition = PitchBendCurrentPosition;                                                 // update pitchbend position
             LastPitchBendEventTime = millis();                                                                    // update timekeeper
             PitchBendValue = pitchBend;                                                                           // update pitchbend value
             return;                                                                                               // exit
        }
        if((PitchBendCurrentPosition > pitch_bend_63) && (PitchBendCurrentPosition < pitch_bend_64)){              // if dead center
             if(PitchBendValue != 0){                                                                              // and last event was in active range
                 midiPitchBend(64);                                                                                // send pitchbend center                                                            
                 PitchBendPreviousPosition = PitchBendCurrentPosition;                                             // update pitchbend position
                 LastPitchBendEventTime = millis();                                                                // update timekeeper
                 PitchBendValue = 0;                                                                               // update pitchbend value
                 return;                                                                                           // exit
            }
        }
        else{
            PitchBendPreviousPosition = PitchBendCurrentPosition;                                             // update pitchbend position
            LastPitchBendEventTime = millis();                                                                // update timekeeper
            PitchBendValue = 0;                                                                               // update pitchbend value        
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

void scanModWheelV2() {

  if(millis() < LastModWheelEventTime){                                           // check for rollover
    return;
  }

  if(millis() > LastModWheelEventTime + modWheel_scan_period){                                                                       // throttle and debounce
    ModWheelCurrentPosition = map(analogRead(modWheel_pin), 0, 1023, 0, mod_wheel_span);                                             // get mod wheel position
    if((ModWheelCurrentPosition >= mod_wheel_dead_bottom) && (ModWheelCurrentPosition <= (mod_wheel_span - mod_wheel_dead_top))){    // if its not a release or in a dead zone
       if(ModWheelCurrentPosition != ModWheelPreviousPosition){                                                                      // and has changed
           midiModWheel(ModWheelCurrentPosition - mod_wheel_dead_bottom);                                                            // send midi
           ModWheelPreviousPosition = ModWheelCurrentPosition;                                                                       // update mod wheel position
           LastModWheelEventTime = millis();                                                                                         // update timekeeper
       }
    }      
  }   
}



///////////////////////////////////////// MIDI MESSAGES //////////////////////////////////////

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
