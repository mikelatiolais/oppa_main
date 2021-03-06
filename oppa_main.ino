#include <SD.h>
#include <SD_t3.h>
#include <QueueArray.h>
#include <i2c_t3.h>
#include "oppa.h"
#include <WS2812Serial.h>

// Globals set up in SD card config
struct OPPA_IO *in_cards;
struct OPPA_IO *out_cards;
struct switch_obj switches[384];
struct solenoid_obj solenoids[384];
struct simple_lamp_obj simple_lamps[256];
struct oppa_lamp_obj oppa_lamps[256];

// These values need to be reset by the SD configuration
byte number_of_switches = 255;
byte number_of_solenoids;
byte number_of_simple_lamps;
byte number_of_rgb_lamps;
byte number_of_in_cards;
byte number_of_out_cards;
// headless will default to 0(controlled by MPF or pinmame), 1 means it just runs with no serial for testing switches/solenoids
byte headless = 0;
// Debounce threshold in ms
unsigned long default_switch_debounce_time = 100;

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

// create a queue of bytes
QueueArray <byte> switch_queue;

// Config file
File myConfig;
const int chipSelect = BUILTIN_SDCARD;
String fileLine;
String sectionName;

// RGB LED Control
// FIXME - Rewrite to be allocated at runtime. 
const int numled = 64;
const int pin = 1;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

String readLine(File myConfig) {
  // Read until we get a newline and return the value
  String line = "";
  while(myConfig.available()) {
    char character = myConfig.read();
    if(character == '\n') {
      break;
    } else {
      line.concat(character);     
    }
  }
  return line;
}

void setup() {
  // Setup Serial to Main Controller
  Serial.begin(9600); //this is wrong FIXME

  // Load configuration from SD Card
  myConfig = SD.open("config");
  if(myConfig) {
    int i = 0;
    fileLine = "";
    sectionName = "";
    while(myConfig.available()) {
      fileLine = readLine(myConfig);
      byte lineChar = myConfig.read();
      if(lineChar == '~') {
        // We have a section header
        // Read the rest of the line until a newline
      } else if(lineChar == '!') {
        // We have an individual value line
      } else if(lineChar != '\n') {
        fileLine.concat(lineChar);
      } else {
        // We have a newline, so do something with the old one.
        // Now, we do an if tree 
      }
    }
  }

  // Set up in_cards
  in_cards = (struct OPPA_IO*)malloc(number_of_in_cards * sizeof(struct OPPA_IO));
  
  // Set up out_cards
  out_cards = (struct OPPA_IO*)malloc(number_of_out_cards * sizeof(struct OPPA_IO));

  // Set up switches
  
}

void loop() {
  byte incoming_byte;
  byte payload;
  byte color;
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
        payload = Serial.read();
        // Read in int for the color
        color = Serial.parseInt();
        oppa_lamps[payload].lighting_timestamp = millis();
        oppa_lamps[payload].pulse = true;
        oppa_lamps[payload].current_state = color;
        break;
      case SET_RGB_LAMP:
        // Do something
        payload = Serial.read();
        // Read in int for the color
        color = Serial.parseInt();
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
      case GET_CHANGED_SWITCHES:
        // Pull the top of the FIFO queue and return it. 
        // The value is the index of the switch.
        // The last possible value, 255, is the null return value. 
        if (!switch_queue.isEmpty()) {
          Serial.print(switch_queue.pop());
        } else {
          Serial.print(255);
        }
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
    byte bus = in_cards[i].i2c_bus;
    byte address = in_cards[i].address;
    byte inputs;
    struct i2c_t3 *i2c_bus;
    if (bus == 0) {
      i2c_bus = &Wire;
    } else if (bus == 1) {
      i2c_bus = &Wire1;
    } else {
      i2c_bus = &Wire2;
    }
    // Read bank A
    for (int bank = 0; bank < 2; bank++) {
      i2c_bus->beginTransmission(address);
      if ( bank == 0 ) {
        i2c_bus->write(0x12); // set MCP23017 memory pointer to GPIOB address
      } else {
        i2c_bus->write(0x13); // set MCP23017 memory pointer to GPIOB address
      }
    
      i2c_bus->endTransmission();
      i2c_bus->requestFrom(0x20, 1); // request one byte of data from MCP20317
      inputs=i2c_bus->read(); // store the incoming byte into "inputs"
      // Split byte and feed into the appropriate switch objects
      for (int bitPosition = 0; bitPosition < 8; bitPosition++) {
        byte bitValue = bitRead(inputs, bitPosition);
        byte switchNumber = (i * 16) + (bank * 8) + bitPosition; 
        // Look at the currnet value of the switch
        if (switches[switchNumber].changing == true && switches[switchNumber].current_val != bitValue && (millis() - switches[switchNumber].last_change) >= default_switch_debounce_time) {
          // The switch has passed the debounce time and it's still changed
          switches[switchNumber].changed = true;
          switches[switchNumber].changing = false;
          switches[switchNumber].current_val = bitValue;
          switches[switchNumber].last_change = 0;
        } else if (switches[switchNumber].changing == true && switches[switchNumber].current_val == bitValue && (millis() - switches[switchNumber].last_change) >= default_switch_debounce_time) {
          // The switch has passed the debounce time and it's back to the original value
          switches[switchNumber].changed = false;
          switches[switchNumber].changing = false;
          switches[switchNumber].last_change = 0;
        } else if (switches[switchNumber].current_val != bitValue && switches[switchNumber].changing == false) {
          // The switch is now showing a possible change
          switches[switchNumber].changing = true;
          switches[switchNumber].changed = false;
          switches[switchNumber].last_change = millis();
        }
      }
    }
    
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


