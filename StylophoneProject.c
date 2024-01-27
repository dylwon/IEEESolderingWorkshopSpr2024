#include <avr/io.h>
#define Pad PA1
#define PitchControl PA2
#define VolumeControl PA3
#define Buzzer PA6
#define LED PA7
#define MAX_VOLT 1023

// Frequency values for each note
#define NOTE_C4 262
#define NOTE_Db4 277
#define NOTE_D4 294
#define NOTE_Eb4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_Gb4 370
#define NOTE_G4 392
#define NOTE_Ab4 415
#define NOTE_A4 440
#define NOTE_Bb4 466
#define NOTE_B4 494
#define NOTE_C5 523

// Voltage values for each note
#define VOLT_C4 0
#define VOLT_Db4 1
#define VOLT_D4 2
#define VOLT_Eb4 3
#define VOLT_E4 4
#define VOLT_F4 5
#define VOLT_Gb4 6
#define VOLT_G4 7
#define VOLT_Ab4 8
#define VOLT_A4 9
#define VOLT_Bb4 10
#define VOLT_B4 11
#define VOLT_C5 12

unsigned int Frequency(int voltage, unsigned int volt_scale[13], unsigned int frequency_scale[13], int pitchShift) {
  
  float lowestError = 500.0, error;
  int index = 0;

  // For-loop calculates which voltage value in the volt_scale is the closest to the measured voltage value using percent error.
  // Error is calculated by taking the float absolute value of of the ((current pad's voltage - measured voltage) / current pad's voltage)) * 100. Values were type casted to ensure error remains a "double" type value.
  for (int i = 0; i < sizeof(volt_scale)/sizeof(unsigned int); i++) {
    error = fabs((volt_scale[i] - voltage)/float(volt_scale[i])) * 100.0;
    if (error < lowestError) { // If the current pad's voltage is the closest to the measured voltage, then we save the error value and index.
      lowestError = error;
      index = i;
    }
  }
  
  // Returns the frequency given by the detected pad +- a percentage of itself.
  // The percentage is defined by calculating the decimal value of the (pitchShift analog reading/MAX_VOLT), and is then multiplied with the current frequency to return an integer value.
  // To ensure that there's no overflow when converting a negative integer value to an unsigned value, we used this if-statement that returns 0 if the integer is negative, and returns the integer value if it's positive.
  if ((frequency_scale[index] + ((pitchShift/(float)MAX_VOLT) * frequency_scale[index])) <= 0.0)
    return 0; 
  else 
    return frequency_scale[index] + ((pitchShift/(float)MAX_VOLT) * frequency_scale[index]);
}

void setup() {
  Serial.begin(9600); // Set baud rate for debugging
  
  // [Initializing Pins as Inputs/Outputs]
  
  // PA1 is the analog input from the resistor ladder
  // PA2 is the analog input from the Pitch Control potentiometer
  // PA3 is the analog input from the Volume Control potentiometer
  DDRA &= (~PIN1 | ~PIN2 | ~PIN3);
  
  // PA6 is the PWM output for the buzzer
  // PA7 is the output to power the indication LED 
  DDRA |= (PIN6 | PIN7);  

  // [Setting the Input/Output Values]

  // Sets the bit at PA7 to be 0 to turn the LED on.
  PORTA &= (~PIN7);
}

void loop() {
  unsigned int frequency_scale[] = {NOTE_C4, NOTE_Db4, NOTE_D4, NOTE_Eb4, NOTE_E4, NOTE_F4, NOTE_Gb4, NOTE_G4, NOTE_Ab4, NOTE_A4, NOTE_Bb4, NOTE_B4, NOTE_C5};
  unsigned int volt_scale[] = {VOLT_C4, VOLT_Db4, VOLT_D4, VOLT_Eb4, VOLT_E4, VOLT_F4, VOLT_Gb4, VOLT_G4, VOLT_Ab4, VOLT_A4, VOLT_Bb4, VOLT_B4, VOLT_C5};
  int voltage;
  int pitchShift;                                                    
  while (1) {
    voltage = analogRead(Pad); // Analog voltage reading at PA1. This value is used to determine which pad was pressed.
    pitchShift = analogRead(PitchControl); // Analog voltage reading at PA2. We use this value to increase/decrease the frequency linearly based on the chosen frequency.
    pitchShift -= MAX_VOLT/2; // We subtract by MAX_VOLT/2 because we want values between 0-516 (0V-2.5V) to lower the pitch, while values of between 517-1023 will raise the pitch (2.5V-5.0V). This is because the potentiometer will be physically centered at 2.5V
    tone(Buzzer, Frequency(analogRead(Pad), volt_scale, frequency_scale, pitchShift));
    noTone();
  }
}
