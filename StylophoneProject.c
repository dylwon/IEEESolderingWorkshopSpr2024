#include <avr/io.h>
#include <util/delay.h>

void PWM_init() {
  
    // [Configure Timer/Counter1 for PWM (Fast PWM mode)]
    // (Fast PWM means it hard resets the counter after comparison)
    
    // COM1B1 configures the timer to be reset after comparison
    // PWM1B ???
    TCCR1A |= (1 << COM1B1) | (1 << PWM1B); 
    
    // WGM11 & WGM10 sets the mode of operation to be fast PWM 
    // CS10 sets Prescaler = 8 [20MHz System Clk/8+1] = 2.222MHz PWM Frequency
    TCCR1B |= (1 << CS10); // Sets prescaler to 8
    TCCR1D |= (1 << WGM11) | (1 << WGM10); // Set Fast PWM mode with OCR1A as the TOP value

    // Initializes the PWM duty cycle to 50% (0 to 255)
    //OCR1A is the TOP value for the timer
    OCR1A = 128;
}

void PWM_out(uint16_t pwm_frequency, uint16_t volume) {
  
    // [Modify Frequency for PWM]

    // Calculate the required prescaler value
    uint16_t prescaler_value;
    if (pwm_frequency <= 0) {
        prescaler_value = 0;  // No prescaling
    } 
    else {
        // Choose a prescaler value that gives a frequency less than or equal to the desired frequency
        prescaler_value = (F_CPU / (2 * pwm_frequency) - 1);
        if (prescaler_value > 16383) {
            prescaler_value = 16383;  // Maximum prescaler value
        }
    }

    // Configure Timer/Counter1 for PWM (Fast PWM mode)
    TCCR1A |= (1 << COM1B1) | (1 << PWM1B); // Set non-inverting mode and Fast PWM mode
    TCCR1B |= ((prescaler_value & 0x07) << CS10); // Set prescaler
    TCCR1D |= (1 << WGM11) | (1 << WGM10); // Set Fast PWM mode with OCR1A as the TOP value
}

void PWMout(unsigned char dutyCycle, float freq) {
  TCCR1A = 0x21; // 0b 0010 0001
  TCCR1B = 0x14; // 0b 0001 0100
  OCR1A = 0x7A12 / freq;
  OCR1B = OCR1A * (dutyCycle / 255.0);
}

void setup() {

  // [Initialization of Pin Directions]
  
  // Sets PA0 as an input
  // PA1 reads the digital voltage value from the resistor ladder
  DDRA |= (0 << PA0);
  
  // Sets PA6 and PA7 as outputs
  // PA6 is the PWM output that feeds into a buzzer
  // PA7 is an indication LED that lights up when the MCU is powered
  DDRA |= (1 << PA6) | (1 << PA7);

  // Sets PB0-PB2 as inputs
  // PA0 is the designated MOSI pin (Master-Out-Slave-In)
  // PA1 is the designated MISO pin (Master-In-Slave-Out)
  // PA2 is the designated SCK pin (Serial Data Clock)
  // DDRB |= (0 << PB0) | (0 << PB1) | (0 << PB2);

  // [Initialization of Pin Values]
  
  // Sets PA7 output to low to turn the indication LED on
  PORTA |= (0 << PA7); 
}

void loop() {
  //Read the digital value of PA0
  //Determine what frequency we need to output to the buzzer using PWM
  //Write a function that modifies the PWM frequency to match the necessary value
}
