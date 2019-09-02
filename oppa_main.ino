#include "oppa.h"
#include <i2c_t3.h>

struct OPPA_IN cards[8];
struct switch_obj switches[20];

void setup() {
  // Setup Serial to Main Controller
  Serial.begin(9600); //this is wrong FIXME

  // Load configuration from SD Card

  
}

void loop() {
  // Check for pending mesages from MPF
  if(Serial.available()) {
    // Read input and process
    // If it is a solenoid pulse, fire the solenoid
    // If it is a light command, change the LED state
  }

  // Loop through input cards and poll each one 
  // Each switch will have an ID. It includes an int indicating that it is an autofire or not.
  // Loop through switches, checking for debounced changes
  // If it's an autofire, immediately fire the associated solenoid(s)
  // Loop through LEDs. Set the expired ones to dark. 
  

  // Update Master with events
  if(Serial.availableForWrite()) {
  
  }
}

byte read_bank() {
  byte value = 0;

  return value;
}

void write_bank() {
  
}

void update_lamps(byte string, int lamp_id) {
  
}

void activate_output(byte card, byte bank, byte output) {
  
}

void queue_event() {
  
}




