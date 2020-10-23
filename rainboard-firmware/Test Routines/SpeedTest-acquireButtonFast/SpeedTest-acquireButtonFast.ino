

const uint8_t number_of_buttons = 61;

boolean ButtonState[number_of_buttons];

const uint8_t scopePin = 53;   // PB0



////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

    pinMode(scopePin, OUTPUT); // scope signal  // PB0                                
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

    acquireButtonsFast(); 

    PORTB |= B00000001; // set PB0
    
    acquireButtonsFast(); 

    PORTB &= B11111110; // clear PB0    
  
}


void acquireButtonsFast(){

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
