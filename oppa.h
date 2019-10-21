#include <i2c_t3.h>

struct OPPA_IO
{
  byte address;
  // i2c_bus will be either 0,1 or 2 -- corresponds to Wire0, Wire1, Wire2
  byte i2c_bus;
  byte banka;
  byte bankb;
};

struct OPPA_OUT
{
  byte address;
  byte i2c_bus;
  byte banka;
  byte bankb;
};

/* 
 *  The new switch numbering scheme
 *  IO cards are setup in an array
 *  So, card 0 has switches 0 - 15
 *  card 1 has switches 16 - 31
 *  etc
 *  card indexes are based on 16(0, 15, 31, 47...)
 *  Each card has 2 banks of 8 bits, so reading bank a is 0-7, bank b is 8-15 
 *  This means the bank index is either 0 or 8
 *  Thus, any switch can be defined by card array index + bank index + position 
 *  switch_obj[0] would be card 0, bank a, position 0
 *  switch_obj[1] would be card 0, bank a, position 1
 *  etc
 *  
 */
struct switch_obj
{
  byte current_val;
  bool changed;
  bool changing;
  unsigned long last_change;
  /* auto_fire_id is the ID of the solenoid which is auto fired when this switch is activated */
  byte auto_fire_id;
};

/*
 * The new solenoid scheme
 * Output cards are stored in an array
 * So, solenoid 0 can be defined as card 0, bank 0, position 0
 * card index = array index * 16
 * bank index = |bank a = 0: bank b = 8| 
 * 
 */
struct solenoid_obj
{
  byte current_val;
  bool is_firing;
  unsigned long firing_timestamp;
  bool enabled;
  bool long_firing;
  bool pulse;
};

struct io_bank
{
  byte card_address;
  byte bank;
  
};

struct simple_lamp_obj
{
  byte lamp_id;
  unsigned long lighting_timestamp;
  bool pulse;
  bool lit;
};

struct oppa_lamp_obj
{
  byte lamp_id;
  byte string_id;
  bool pulse;
  int current_state;
  unsigned long lighting_timestamp;
};

String readLine(File myConfig);

/* Serial Protocol */
/* The protocol is based on the LISY protocol used in MPF */
/* It will also be extended to allow for communication with pinmame */

/* GET_CONNECTED_HARDWARE - return 1 byte showing board type as defined below 
 * 0x00 - OPPA Teensy v1
 * 0x01 - OPPA Mega v1 
 */
#define GET_CONNECTED_HARDWARE 0x00

/* GET_FIRMWARE_VERSION - return 1 byte showing firmware version as defined below 
 * 0x00 - v0.1 
 */
#define GET_FIRMWARE_VERSION  0x01

/* GET_API_VERSION - return 1 byte showing API version as defined below
 * 0x00 - v0.1
 * I don't actually expect to use this much
 */
#define GET_API_VERSION  0x02

/* GET_SIMPLE_LAMP_COUNT - returns 1 byte for number of simple lamps */
#define GET_SIMPLE_LAMP_COUNT  0x03

/* GET_SOLENOID_COUNT - returns 1 byte for number of solenoids */
#define GET_SOLENOID_COUNT  0x04

/* GET_SOUND_COUNT - returns 1 byte for umber of sounds - UNUSED */
#define GET_SOUND_COUNT  0x05

/* GET_SEGMENT_DISPLAY_COUNT - returns 1 byte */
#define GET_SEGMENT_DISPLAY_COUNT  0x06

/* GET_SEGMENT_DISPLAY_DETAILS - payload of 1 byte, returns 2 bytes */
#define GET_SEGMENT_DISPLAY_DETAILS  0x07

/* GET_GAME_INFO - returns 1 byte */
#define GET_GAME_INFO  0x08

/* GET_SWITCH_COUNT - returns 1 byte */
#define GET_SWITCH_COUNT  0x09

/* GET_STATUS_OF_SIMPLE_LAMP - returns 1 byte */
#define GET_STATUS_OF_SIMPLE_LAMP  0x0A

/* SET_SIMPLE_LAMP_TO_ON - no return */
#define SET_SIMPLE_LAMP_TO_ON  0x0B

/* SET_SIMPLE_LAMP_TO_OFF - no return */
#define SET_SIMPLE_LAMP_TO_OFF  0x0C

/* GET_STATUS_OF_SOLENOID - returns 1 byte */
#define GET_STATUS_OF_SOLENOID  0x14

/* ENABLE_SOLENOID_FULL_POWER - no return */
#define ENABLE_SOLENOID_FULL_POWER  0x15

/* DISABLE_SOLENOID - no return */
#define DISABLE_SOLENOID  0x16

/* PULSE_SOLENOID - no return */
#define PULSE_SOLENOID  0x17

/* PULSE_RGB_LAMP - no return, followed by int for color */
#define PULSE_RGB_LAMP  0x0D

/* PULSE_SIMPLE_LAMP - no return */
#define PULSE_SIMPLE_LAMP  0x0E

/* SET_SOLENOID_PULSE_TIME - no return */
#define SET_SOLENOID_PULSE_TIME  0x18

/* SET_SEGMENT_DISPLAY - UNUSED */
#define SET_SEGMENT_DISPLAY  0x1E

/* SET_RGB_LAMP - no return, followed by int for color */
#define SET_RGB_LAMP  0x0F

/* GET_SWITCH_STATUS - returns 1 byte */
#define GET_SWITCH_STATUS  0x28

/* GET_CHANGED_SWITCHES - who knows? */
#define GET_CHANGED_SWITCHES  0x29

/* INIT_RESET - reset and initialize the platform */
#define INIT_RESET  0x64

/* WATCHDOG - disable all solenoids and lights if the watchdog is not sent at within 1 second */
#define WATCHDOG  0x65

/* GET_RGB_LAMP_COUNT - returns count of RCG lamps */
#define GET_RGB_LAMP_COUNT  0x13

/* COMMAND_PREFIX - The prefix that determines that the next byte is a command */
#define COMMAND_PREFIX  0x7E

/* SD Card Format 
 *  
 *  <signifier><value string>
 * 
 *  inputs are prepended with a ~ signifier
 *  solenoids are prepended with a ^ signifier
 *  Any individual vaue will be prepended with !
 *  
 *  Actual data example
 *  ~0,0   (switch #, autofire_id)
 *  ~1,0
 *  ~2,0
 *  ~3,1
 *  ^0,0  (solenoid #, long firing)
 *  !RGB_LAMPS:64   (Just the number of lamps)
 *  !HEADLESS:1 (headless mode means no MPF controller. Set for testing)
 */



/* GET_SER_NUM_CMD = b'\x00'
    GET_PROD_ID_CMD = b'\x01'
    GET_VERS_CMD = b'\x02'
    SET_SER_NUM_CMD = b'\x03'
    RESET_CMD = b'\x04'
    GO_BOOT_CMD = b'\x05'
    CFG_SOL_CMD = b'\x06'
    KICK_SOL_CMD = b'\x07'
    READ_GEN2_INP_CMD = b'\x08'
    CFG_INP_CMD = b'\x09'
    SAVE_CFG_CMD = b'\x0b'
    ERASE_CFG_CMD = b'\x0c'
    GET_GEN2_CFG = b'\x0d'
    SET_GEN2_CFG = b'\x0e'
    CHNG_NEO_CMD = b'\x0f'
    CHNG_NEO_COLOR = b'\x10'
    CHNG_NEO_COLOR_TBL = b'\x11'
    SET_NEO_COLOR_TBL = b'\x12'
    INCAND_CMD = b'\x13'
    CFG_IND_SOL_CMD = b'\x14'
    CFG_IND_INP_CMD = b'\x15'
    SET_IND_NEO_CMD = b'\x16'
    SET_SOL_INP_CMD = b'\x17'
    UPGRADE_OTHER_BRD = b'\x18'
    READ_MATRIX_INP = b'\x19'

    INV_CMD = b'\xf0'
    ILLEGAL_CMD = b'\xfe'
    EOM_CMD = b'\xff'

    CARD_ID_TYPE_MASK = b'\xf0'
    CARD_ID_GEN2_CARD = b'\x20'

    NUM_G2_WING_PER_BRD = 4
    WING_SOL = b'\x01'
    WING_INP = b'\x02'
    WING_INCAND = b'\x03'
    WING_SW_MATRIX_OUT = b'\x04'
    WING_SW_MATRIX_IN = b'\x05'
    WING_NEO = b'\x06'
    WING_HI_SIDE_INCAND = b'\x07'

    NUM_G2_INP_PER_BRD = 32
    CFG_INP_STATE = b'\x00'
    CFG_INP_FALL_EDGE = b'\x01'
    CFG_INP_RISE_EDGE = b'\x02'

    # Solenoid configuration constants
    CFG_BYTES_PER_SOL = 3
    INIT_KICK_OFFSET = 1
    DUTY_CYCLE_OFFSET = 2
    NUM_G2_SOL_PER_BRD = 16

    CFG_SOL_DISABLE = b'\x00'
    CFG_SOL_USE_SWITCH = b'\x01'
    CFG_SOL_AUTO_CLR = b'\x02'
    CFG_SOL_ON_OFF = b'\x04'
    CFG_SOL_DLY_KICK = b'\x08'
    CFG_SOL_USE_MTRX_INP = b'\x10'
    CFG_SOL_CAN_CANCEL = b'\x20'

    CFG_SOL_INP_REMOVE = b'\x80'

    NUM_COLOR_TBL = 32
    NEO_CMD_ON = 0x80

    INCAND_ROT_LEFT = b'\x00'
    INCAND_ROT_RIGHT = b'\x01'
    INCAND_LED_ON = b'\x02'
    INCAND_LED_OFF = b'\x03'
    INCAND_BLINK_SLOW = b'\x04'
    INCAND_BLINK_FAST = b'\x05'
    INCAND_BLINK_OFF = b'\x06'
    INCAND_SET_ON_OFF = b'\x07'

    INCAND_SET_CMD = b'\x80'
    INCAND_SET_ON = b'\x01'
    INCAND_SET_BLINK_SLOW = b'\x02'
    INCAND_SET_BLINK_FAST = b'\x04'

 */
