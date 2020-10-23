

const uint8_t number_of_buttons = 61;

boolean ButtonState[number_of_buttons];

const uint8_t  button_to_pin[] = 
{31,35,38,42,46,30,32,36,40,44,51,26,29,33,39,45,48,50,22,25,28,34,41,49,52,A13,17,18,24,27,37,47,A15,A12,A11,14,16,19,23,43,A14,A8,A7,5,4,15,6,A4,A6,A5,11,10,9,7,A1,A3,12,13,8,A0,A2}; 

const uint8_t scopePin = 53;   // PB0



////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

    pinMode(scopePin, OUTPUT); // scope signal  // PB0    

    for(uint8_t i=0; i<number_of_buttons; i++){  
        pinMode(button_to_pin[i], INPUT_PULLUP);     
    }
                                
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

    acquireButtonsSlow(); 

    PORTB |= B00000001; // set PB0
    
    acquireButtonsSlow(); 

    PORTB &= B11111110; // clear PB0    
  
}


void acquireButtonsSlow(){
    for(uint8_t i=0; i<number_of_buttons; i++){ 
      ButtonState[i] = (digitalRead(button_to_pin[i]));       
   } 
}
