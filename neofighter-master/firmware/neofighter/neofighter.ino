#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

int mem[] = {0,0,0,0};

void setup() {

  /* Pin Configuration
   * btn: 1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
   * pin: D0  B7  F4  F5  D1  D5  F7  F6  D6  D4  C7  C6  B4  D7  B6  B5
   */
    
  DDRB  &= ~(1<<7 | 1<<6 | 1<< 5 | 1<<4);
  PORTB |=  (1<<7 | 1<<6 | 1<< 5 | 1<<4);
  DDRC  &= ~(1<<7 | 1<<6);
  PORTC |=  (1<<7 | 1<<6);
  DDRD  &= ~(1<<7 | 1<<6 | 1<< 5 | 1<<4 | 1<<1 | 1<<0);
  PORTD |=  (1<<7 | 1<<6 | 1<< 5 | 1<<4 | 1<<1 | 1<<0);
  DDRF  &= ~(1<<7 | 1<<6 | 1<< 5 | 1<<4);
  PORTF |=  (1<<7 | 1<<6 | 1<< 5 | 1<<4);

//  pixels.begin();
  Serial.begin(115200);
  
}


void loop(){
  int sensorVal[] = {
    !digitalRead(2), !digitalRead(3),
    !digitalRead(4), !digitalRead(5), !digitalRead(6)
  };

for(int j = 0 ; j<5; j++){
  if (sensorVal[j] != mem[j]){
    mem[j] = sensorVal[j];
    if(mem[j]){
      MIDI.sendNoteOn(j+48, 64, 1);
//        Serial.print("test");
        Serial.print(j);


    }else{
      MIDI.sendNoteOff(j+48, 64, 1);
//        Serial.print("test");
        Serial.print(j);
    }
  }
}


}
