/***************************************************************
 * This example sketch demonstrates the usage of shift registers
 * for simultaneous 8 bit input and 8 bit output using a total
 * of 3 pins on an arduino. This code has been tested with both
 * an Arduino Uno and an ATTiny85
 *
 * @author Kevin Vece
 ***************************************************************/

/***************************************************************
 * How to use:
 * - Define your pin setup directly below this block
 * - Then scroll down to the loop() function and define your actions
 *   in the if statements where it says "ACTION HERE"
 *     - There are two if blocks, one determines if the button[j]
 *       was just pressed, and the other determines if button[j]
 *       was just released
 * - Enjoy! :)
 ***************************************************************/

/*
 * Preset Pins to use on board for ShiftOut function (defined below)
 * Arduino Uno Pin 8: 1
 * Arduino Uno Pin 9: 2
 * Arduino Uno Pin 10: 4
 * Arduino Uno Pin 11: 8
 * Arduino Uno Pin 12: 16
 * Arduino Uno Pin 13: 32
 *
 * ATTiny25/45/85 (Pin Numbers are starting from dot)
 * ATTiny Pin 2: 8
 * ATTiny Pin 3: 16
 * ATTiny Pin 5: 1
 * ATTiny Pin 6: 2
 * ATTiny Pin 7: 4
 *
 *
 * On an Arduino Uno:
 * For example, CLOCK is defined as Pin 12 below,
 *              DATA is pin 11,
 *              BUTTONS (where the buttons input to), is pin 9,
 *              LATCH is pin 8
 */
#define CLOCK 16
#define DATA 8
#define BUTTONS 2
#define LATCH 1

// Uncomment this line if you dont' change the pin setup
#define VC_PINS 1414

// Uncomment this line if you use hardware debouncing
//#define VC_HARDWARE_DEBOUNCE 1414

// This is the number of input pins (for now this must be less than 8)
#define INPUTS 8

// output contains the output shift register values
byte output = 0;

// these are used for software debouncing
boolean inputs[INPUTS];
#ifndef VC_HARDWARE_DEBOUNCE
int counter[INPUTS];
#define DEBOUNCE_THRESHOLD 50
#endif

/*****************************************************
* Usual Arduino code sections for setup() and loop() *
*****************************************************/

void setup() {
  // set pins PB0, PB3, and PB4 up for output
  // if you used the same pins as me, uncomment #define VCOPT for slightly quicker setup
  #ifdef VC_PINS
    DDRB = B00011001;
  #else
    DDRB = B00000000;
    DDRB |= CLOCK | DATA | LATCH;
  #endif

  // set all output to low
  PORTB = B00000000;

  // shift initial values to registers
  ShiftOut(255);
  ShiftOut(output);
  Latch();
}

void loop() {

  if (READ(BUTTONS)) { // a button was pressed
  
    // let's poll each button individually to see which buttons are down
    byte i = B00000001;
    for(int j = 0; j < INPUTS; ++j, i = i<<1) {
      ShiftOut(i);
      ShiftOut(output);
      Latch();
      if (READ(BUTTONS)) {
        
        #ifndef VC_HARDWARE_DEBOUNCE
        // check to see if button has stabilized
        if (++counter[j] >= DEBOUNCE_THRESHOLD) {
        #endif
          
          // check if button was already down and mark it's new state
          if (!inputs[j] && (inputs[j] = true)) {
            // ACTION HERE (button was just pressed)
            
            // this action toggles the output value
            output ^= i;
            
          }
          
          #ifndef VC_HARDWARE_DEBOUNCE
          // let's keep the counter from overflowing
          counter[j] = DEBOUNCE_THRESHOLD;
        }
        #endif
      }
      else {
       
        #ifndef VC_HARDWARE_DEBOUNCE
        //check to see if button has stabilized 
        if (--counter[j] < 0) {
        #endif
            
          // check if button was already up and mark it's new state
          if (inputs[j] && (inputs[j] = false)) {
            //ACTION HERE (button was just released)
            
          }
          
          #ifndef VC_HARDWARE_DEBOUNCE
          // let's keep the counter from underflowing
          counter[j] = 0;
        }
        #endif
      }
    }

    // send all ones to the input shift register and our output to the output register
    ShiftOut(255);
    ShiftOut(output);
    Latch();
  }
  else {
    
    // software debounce counter
    for(int j = 0; j < INPUTS; ++j) {
      #ifndef VC_HARDWARE_DEBOUNCE
      if (--counter[j] < 0) {
      #endif
        inputs[j] = false;
        
        #ifndef VC_HARDWARE_DEBOUNCE
        counter[j] = 0;
      }
      #endif
    }
  }

}

/***************************************************************
* This section contains some inline functions for optimization *
***************************************************************/

inline void SET(byte _pin) {
  PORTB |= _pin;
}

inline void CLEAR(byte _pin) {
  PORTB &= ~(_pin);
}

inline void TOGGLE(byte _pin) {
  PORTB ^= (_pin); 
}

inline byte READ(byte _pin) {
  return PINB & _pin;
}

inline void ShiftOut(byte data) {
  // set clock and latch low
  CLEAR(CLOCK);
  CLEAR(LATCH);

  for(uint8_t i = 0; i < 8; ++i) {
    //check the content
    if ((data << i) & B10000000) {
      // set data pin high
      SET(DATA);
    }
    else {
      // set data pin low 
      CLEAR(DATA);
    }
    // toggle clock twice
    TOGGLE(CLOCK);
    TOGGLE(CLOCK);
  }

}

inline void Latch() {
  // latch it
  CLEAR(LATCH);
  SET(LATCH);
  delayMicroseconds(10);
}

