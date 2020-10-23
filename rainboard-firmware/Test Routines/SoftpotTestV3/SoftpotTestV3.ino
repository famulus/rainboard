

const uint8_t global_midi_channel = 1;

const uint8_t midi_cc_command = 0xB0;
const uint8_t midi_pitchBend_cmd = 0xE0;
const uint8_t midi_modWheel_number = 0x01;

const uint8_t number_of_buttons = 61;

const uint8_t modWheel_pin = A9;             
const uint8_t pitchBend_pin = A10;  
const uint8_t softpots_bottom_pin = A11;           


//scanModWheel()
uint8_t ModWheelCurrentPosition;
uint8_t ModWheelPreviousPosition;
uint64_t LastModWheelEventTime;


//scanPitchBend()
uint8_t PitchBendCurrentPosition;
uint8_t PitchBendPreviousPosition;
uint8_t PitchBendValue;
uint64_t LastPitchBendEventTime;

// configure timing
const uint64_t dead_zone_time = 2;

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
                                         // init softpots
  pinMode(modWheel_pin, INPUT);
  ModWheelCurrentPosition = ModWheelPreviousPosition = 0;    
  pinMode(pitchBend_pin, INPUT);  
  PitchBendCurrentPosition = PitchBendPreviousPosition = 0;
  
  LastModWheelEventTime = LastPitchBendEventTime = millis(); // init timekeepers
  PitchBendValue = 0;

  pinMode(softpots_bottom_pin, INPUT);
  analogReference(EXTERNAL);
     
  
  Serial.begin(9600);                 // init debug port    
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {


  //checkSoftpots(); 

  //scanModWheelV1();

  scanPitchBendV1();

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

void scanPitchBendV1() {

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
            //midiPitchBend(PitchBendCurrentPosition);                                          // send midi
            Serial.print("A10 PITCH = ");
            Serial.println(PitchBendCurrentPosition);
            PitchBendPreviousPosition = PitchBendCurrentPosition;                    // update pitch bend value
            LastPitchBendEventTime = millis();                                       // update timekeeper
         } 
      }
    }      
  }   
}


void scanModWheelV1() {

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
             //midiModWheel(ModWheelCurrentPosition);                                          // send midi
             Serial.print("A9 MOD = ");
             Serial.println(ModWheelCurrentPosition);
             ModWheelPreviousPosition = ModWheelCurrentPosition;                    // update mod wheel value
             LastModWheelEventTime = millis();                                      // update timekeeper
          } 
       }
    }      
  }   
}




/*
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


void scanModWheelV3() {

  static uint16_t mod_wheel_raw;

  if(millis() < LastModWheelEventTime){                                           // check for rollover
    return;
  }

  if(millis() > LastModWheelEventTime + modWheel_scan_period){                    // throttle and debounce
    
    mod_wheel_raw = analogRead(modWheel_pin);
    if(mod_wheel_raw > modWheel_noise_floor){                                              // if its not a release
       ModWheelCurrentPosition = map(mod_wheel_raw, analogRead(softpots_bottom_pin), 1023, 0, 128);     // get mod wheel value
       if(ModWheelCurrentPosition != ModWheelPreviousPosition){                   // and has changed
           //midiModWheel(ModWheelCurrentPosition - 1);                             // send midi
           Serial.print("A9 MOD = ");
           Serial.println(ModWheelCurrentPosition - 1);
           ModWheelPreviousPosition = ModWheelCurrentPosition;                    // update mod wheel value
           LastModWheelEventTime = millis();                                      // update timekeeper
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

*/
