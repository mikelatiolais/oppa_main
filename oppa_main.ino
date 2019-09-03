#include <QueueArray.h>
#include <i2c_t3.h>
#include "oppa.h"

// Globals set up in SD card config
struct OPPA_IN cards[8];
struct switch_obj switches[256];

// These values need to be reset by the SD configuration
byte number_of_switches = 0;
byte number_of_solenoids = 0;

byte hardware_type = 0x00;
byte firmware_version = 0x00;


// Used to enable/disable ALL solenoids
// For example, when the coin door is open 
bool solenoids_enabled = false;

// Used to enable/disable the flippers
bool flippers_enabled = false;

// create a queue of strings
QueueArray <String> switch_queue;

void setup() {
  // Setup Serial to Main Controller
  Serial.begin(9600); //this is wrong FIXME

  // Load configuration from SD Card

  
}

void loop() {
  byte incoming_byte;
  byte payload;
  // Check for pending messages from MPF
  while(Serial.available()) {
    // Read input and process
    incoming_byte = Serial.read();
    switch(incoming_byte) {
      case GET_CONNECTED_HARDWARE:
        // Return 
        Serial.print(hardware_type);
        break; 
      case GET_FIRMWARE_VERSION:
        Serial.print(firmware_version);
        break;
      case GET_SIMPLE_LAMP_COUNT:
        // Do something
        break;
      case GET_SOLENOID_COUNT:
        Serial.print(number_of_solenoids);
        break;
      case GET_SWITCH_COUNT:
        Serial.print(number_of_switches);
        break;
      case GET_STATUS_OF_SIMPLE_LAMP:
        // Do something
        break;
      case SET_SIMPLE_LAMP_TO_ON:
        // Do something
        break;
      case SET_SIMPLE_LAMP_TO_OFF:
        // Do something
        break;
      case GET_STATUS_OF_SOLENOID:
        // Do something
        break;
      case ENABLE_SOLENOID_FULL_POWER:
        // Do something
        break;
      case DISABLE_SOLENOID:
        // Do something
        break;
      case PULSE_SOLENOID:
        // Do something
        break;
      case SET_SOLENOID_PULSE_TIME:
        // Do something
        break;
      case GET_SWITCH_STATUS:
        // Get payload
        payload = Serial.read();
        Serial.print(switches[payload].current_val);
        break; 
          
    }
    
  }

  // Loop through input cards and poll each one 
  // Each switch will have an ID. It includes an int indicating that it is an autofire or not.
  // Loop through switches, checking for debounced changes
  // If it's an autofire, immediately fire the associated solenoid(s)
  // Loop through LEDs. Set the expired ones to dark. 
  

  
}

byte process_bank() {
  byte value = 0;

  return value;
}

void write_bank() {
  
}

void update_lamps(byte string, byte lamp_id) {
  
}

void activate_output(byte card, byte bank, byte output) {
  
}

void queue_event() {
  
}

void process_event() {
  
}



