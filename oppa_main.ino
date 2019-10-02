#include <SD.h>
#include <SD_t3.h>
#include <QueueArray.h>
#include <i2c_t3.h>
#include "oppa.h"
#include <WS2812Serial.h>

// Globals set up in SD card config
struct OPPA_IO *in_cards;
struct OPPA_IO *out_cards;
struct switch_obj *switches;
struct solenoid_obj *solenoids;
struct simple_lamp_obj *simple_lamps;
struct oppa_lamp_obj *oppa_lamps;

// These values need to be reset by the SD configuration
byte number_of_switches;
byte number_of_solenoids;
byte number_of_simple_lamps;
byte number_of_rgb_lamps;
byte number_of_in_cards;
byte number_of_out_cards;
// headless will default to 0(controlled by MPF or pinmame), 1 means it just runs with no serial for testing switches/solenoids
byte headless = 0;

// The default time that a solenoid is activated
unsigned long solenoid_pulse_time;

// The default time that a lamp is allowed to fade
unsigned long lamp_fade_time;
unsigned long lamp_pulse_time;


byte hardware_type;
byte firmware_version;
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
String fileLine;

// RGB LED Control
// FIXME - Rewrite to be allocated at runtime. 
const int numled = 64;
const int pin = 1;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);


void setup() {
  // Setup Serial to Main Controller
  Serial.begin(9600); //this is wrong FIXME

  // Load configuration from SD Card
  myConfig = SD.open("config");
  if(myConfig) {
    int i = 0;
    while(myConfig.available()) {
      byte lineChar = myConfig.read();
      if(lineChar != '\n') {
        fileLine[i] = myConfig.read();
      }
    }
  }
  
}

void loop() {
  byte incoming_byte;
  byte payload;
  // Check for pending messages from MPF
  while(Serial.available()) {
    // Read input and process
    incoming_byte = Serial.read();
    // If a byte is waiting and it's not the command prefix, then it's junk. Toss it and try again.
    if(incoming_byte == COMMAND_PREFIX) {
      continue;
    } else {
      // Grab the next byte, which should be valid
      incoming_byte = Serial.read();
    }
    
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
        if(simple_lamps[payload].lit) {
          Serial.print(0x01);
        } else {
          Serial.print(0x00);
        }
        break;
      case SET_SIMPLE_LAMP_TO_ON:
        // Do something
        payload = Serial.read();
        simple_lamps[payload].lit = true;
        break;
      case SET_SIMPLE_LAMP_TO_OFF:
        // Do something
        payload = Serial.read();
        simple_lamps[payload].lit = false;
        break;
      case GET_STATUS_OF_SOLENOID:
        // Do something
        payload = Serial.read();
        if(solenoids[payload].is_firing) {
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
        payload = Serial.read();
        if(solenoids[payload].enabled) {
          solenoids[payload].firing_timestamp = millis();
          solenoids[payload].pulse = true;
        } 
        break;
      case PULSE_RGB_LAMP:
        // Do something
        byte color;
        payload = Serial.read();
        // Read in int for the color
        color = Serial.parseint();
        oppa_lamps[payload].lighting_timestamp = millis();
        oppa_lamps[payload].pulse = true;
        oppa_lamps[payload].current_state = color;
        break;
      case SET_RGB_LAMP:
        // Do something
        byte color;
        payload = Serial.read();
        // Read in int for the color
        color = Serial.parseint();
        oppa_lamps[payload].lighting_timestamp = millis();
        oppa_lamps[payload].pulse = false;
        oppa_lamps[payload].current_state = color;
        break;
      case PULSE_SIMPLE_LAMP:
        // Do something
        payload = Serial.read();
        simple_lamps[payload].lighting_timestamp = millis();
        simple_lamps[payload].pulse = true;
        break;
      case SET_SOLENOID_PULSE_TIME:
        // How many milliseconds should the pulse time be?
        payload = Serial.read();
        solenoid_pulse_time = payload;
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
  for(byte i = 0; i < number_of_in_cards; i++) {
    // Get address
    //in_cards[i].address;
    
  }
   
  // Each switch will have an ID. It includes an int indicating that it is an autofire or not.
  // Loop through switches, checking for debounced changes
  // If it's an autofire, immediately fire the associated solenoid(s)

  // Loop through LEDs. Set the expired ones to dark. 
  for(byte i = 0; i < number_of_simple_lamps; i++) {
    
  }
  
  
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


