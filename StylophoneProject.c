#include <avr/io.h>
#define PITCHCONTROL PA4
#define VolumeControl PA5
#define BUZZER PA6
#define LED PA7
#define MAX_VOLT 1023
#define CUTOFF_VOLT 1000

// Frequency values for each note
//#include "pitches.h"

#define NOTE_C4  100
#define NOTE_CS4 100
#define NOTE_D4  2500
#define NOTE_DS4 2500
#define NOTE_E4  1000
#define NOTE_F4  1000
#define NOTE_FS4 2500
#define NOTE_G4  2500
#define NOTE_GS4 2000
#define NOTE_A4  2000
#define NOTE_AS4 2500
#define NOTE_B4  2500
#define NOTE_C5  2500

unsigned int frequency_scale[] = {NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5};
unsigned int volt_scale[] = {0, 694, 882, 955};
unsigned short int inputs[] = {PA0, PA1, PA2, PA3};
int voltage;
int pitchShift;

int index = 0;


void setup() {
  //Serial.begin(9600); // Set baud rate for debugging
 
  // [Initializing Pins as Inputs/Outputs]
 
  // PA0-2 is the analog input from resistor ladder
  // PA3 is the digital input from a single pad
  // PA4
  // PA5
  DDRA &= (0 << PA0) | (0 << PA1) |(0 << PA2) |(0 << PA3);
 
  // PA6 is the PWM output for the buzzer
  // PA7 is the output to power the indication LED
  DDRA |= (1 << PA4) |(1 << PA5) | (1 << PA6) |(1 << PA7);

  // [Setting the Input/Output Values]

  // Sets the bit at PA7 to be 0 to turn the LED on.
  PORTA &= (0 << PA7) | (0 << PA4) | (0 << PA5);

  PORTA |= (1 << PA4) | (1 << PA5);

//  for (int i = 0; i < 14; i++) {
//    tone(BUZZER, frequency_scale[i]);
//    delay(400);
//    noTone(BUZZER);
//    delay(200);
//  }
}

unsigned int frequency(int pad) {
  int error;
  int lowestError = 500;

  if (pad == 3)
     return NOTE_C5;
 
  // For-loop calculates which voltage value in the volt_scale is the closest to the measured voltage value using percent error.
  // Error is calculated by taking the float absolute value of of the ((current pad's voltage - measured voltage) / current pad's voltage)) * 100. Values were type casted to ensure error remains a "double" type value.
  unsigned int* ptr = &volt_scale[0];
  for (unsigned short int i = 0; i < 4; i++) {
     error = abs((*ptr)++ - (unsigned int)voltage);
     if (error < lowestError) {
       lowestError = error;
       index = pad*4 + i; // INDEX IS ALWAYS PAD*4 FOR SOME REASON
     }
  }
 
  PORTA |= (1 << PA4) | (1 << PA5);
  switch(index%4) {
    case 1: PORTA &= ((0 << PA4)); delay(500); break;
    case 2: PORTA &= ((0 << PA5)); delay(500); break;
    case 3: PORTA &= ((0 << PA4) | (0 << PA5)); delay(500); break;
    default: PORTA |= ((1 << PA4) | (1 << PA5)); delay(100);
  }
 
  //  switch(index) {
  //    case 1: PORTA |= (1 << PA5) | (1 << PA4); PORTA &= ~(1 << PA4); break;
  //    case 2: PORTA |= (1 << PA5) | (1 << PA4); PORTA &= ~(1 << PA5); break;
  //    case 3: PORTA |= (1 << PA5) | (1 << PA4); PORTA &= ~(1 << PA4) | (1 << PA5); break;
  //    default: break;
  //  }

  // Returns the frequency given by the detected pad +- a percentage of itself.
  // The percentage is defined by calculating the decimal value of the (pitchShift analog reading/MAX_VOLT), and is then multiplied with the current frequency to return an integer value.
  //return (unsigned short int) (frequency_scale[index] + (unsigned int)((pitchShift/(float)MAX_VOLT) * frequency_scale[index]));
  //freqSelect = frequency_scale[index] + pitchShift;
  return frequency_scale[index];

}


void loop() {
  int freqSelect;
  while (1) {
    for (int i = 0; i < 4; i++) {
      //pitchShift = analogRead(PITCHCONTROL);
      //If nothing is pressed, skip that pin and look at the next pin
     
      while (analogRead(inputs[i]) < CUTOFF_VOLT ^ (i == 3 && digitalRead(PA3) == 0)){
          freqSelect = frequency(i);
          tone(BUZZER, freqSelect);
        }
       
     }
    tone(BUZZER, 0);
  }
}
