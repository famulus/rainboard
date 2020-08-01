//Allocate the buttons to the digi IOs on the board, pins 2-7
#define PITCH_1 0x3F //D#



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