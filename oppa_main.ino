#include <SD.h>
#include <SD_t3.h>
#include <QueueArray.h>
#include <i2c_t3.h>
#include "oppa.h"
#include <WS2812Serial.h>

// Globals set up in SD card config
struct OPPA_IO in_cards[24];
struct OPPA_IO out_cards[24];
struct switch_obj switches[256];
struct solenoid_obj solenoids[256];
struct simple_lamp_obj simple_lamps[256];
struct oppa_lamp_obj oppa_lamps[256];

// These values need to be reset by the SD configuration
byte number_of_switches = 0;
byte number_of_solenoids = 0;
byte number_of_simple_lamps = 0;
byte number_of_rgb_lamps = 0;

byte hardware_type = 0x00;
byte firmware_version = 0x00;
unsigned long watchdog_timer;


// Used to enable/disable ALL solenoids
// For example, when the coin door is open 
bool solenoids_enabled = false;

// Used to enable/disable the flippers
bool flippers_enabled = false;

// create a queue of strings
QueueArray <String> switch_queue;

// Config file
File myConfig;
const int chipSelect = BUILTIN_SDCARD;

void setup() {
  // Setup Serial to Main Controller
  Serial.begin(9600); //this is wrong FIXME

  // Load configuration from SD Card
  myConfig = SD.open("config");
  if(myConfig) {
    
  }
  
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
        Serial.print(hardware_type);
        break; 
      case GET_FIRMWARE_VERSION:
        Serial.print(firmware_version);
        break;
      case GET_SIMPLE_LAMP_COUNT:
        Serial.print(number_of_simple_lamps);
        break;
      case GET_SOLENOID_COUNT:
        Serial.print(number_of_solenoids);
        break;
      case GET_SWITCH_COUNT:
        Serial.print(number_of_switches);
        break;
      case GET_STATUS_OF_SIMPLE_LAMP:
        // Do something
        payload = Serial.read();
        Serial.print(simple_lamps[payload]);
        break;
      case SET_SIMPLE_LAMP_TO_ON:
        // Do something
        payload = Serial.read();
        simple_lamps[payload] = true;
        break;
      case SET_SIMPLE_LAMP_TO_OFF:
        // Do something
        payload = Serial.read();
        simple_lamps[payload] = false;
        break;
      case GET_STATUS_OF_SOLENOID:
        // Do something
        payload = Serial.read();
        if(solenoids[payload]) {
          Serial.print(0x01);
        } else {
          Serial.print(0x00);
        } 
        break;
      case ENABLE_SOLENOID_FULL_POWER:
        // Do something
        payload = Serial.read();
        solenoids[payload].enabled = true;
        break;
      case DISABLE_SOLENOID:
        // Do something
        payload = Serial.read();
        solenoids[payload].enabled = false;
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
      case INIT_RESET:
        // Initialize/Reset board
        reset_board();
        break;
      case WATCHDOG:
        // Reset watchdog_timer
        watchdog_timer = millis(); 
        break;
    }
    
  }

  // Loop through input cards and poll each one
   
  // Each switch will have an ID. It includes an int indicating that it is an autofire or not.
  // Loop through switches, checking for debounced changes
  // If it's an autofire, immediately fire the associated solenoid(s)
  // Loop through LEDs. Set the expired ones to dark. 
  
  // Check watchdog_timer. If it is over 1 second, reset the board
  if(millis() - watchdog_timer > 1000) {
    reset_board();
  }
  
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

// Reset the board
void reset_board() {
  // Turn off all solenoids
  // Turn off all simple lamps
  // Turn off all RGB lamps
  // Reset all switch states to 0

  // Set watchdog_timer to zero
  // This will cause a continuous reset until a watchdog command is issued again
  watchdog_timer = 0;
}


