#include <i2c_t3.h>

struct OPPA_IN
{
  int address;
  int i2c_bus;
  int banka;
  int bankb;
  int input;
};

struct OPPA_OUT
{
  int address;
  int i2c_bus;
  int banka;
  int bankb;
};

struct switch_obj
{
  int current_val;
  int changing;
  unsigned long last_change;
  struct OPPA_IO *card;
  int bank;
  int bit_position;
  int auto_fire_id;
};

struct io_bank
{
  byte card_address;
  int bank;
  
};

struct oppa_lamp
{
  int lamp_id;
  byte string_id;
};

/* Serial Protocol */
/* The protocol is based on the LISY protocol used in MPF */
/* It will also be extended to allow for communication with pinmame */

/* GET_CONNECTED_HARDWARE - return 1 byte showing board type as defined below 
 * 0x00 - OPPA Teensy v1
 * 0x01 - OPPA Mega v1 
 */
#define GET_CONNECTED_HARDWARE b'\x00'

/* GET_FIRMWARE_VERSION - return 1 byte showing firmware version as defined below 
 * 0x00 - v0.1 
 */
#define GET_FIRMWARE_VERSION  b'\x01'

/* GET_API_VERSION - return 1 byte showing API version as defined below
 * 0x00 - v0.1
 * I don't actually expect to use this much
 */
#define GET_API_VERSION  b'\x02'

/* GET_SIMPLE_LAMP_COUNT - returns 1 byte for number of simple lamps */
#define GET_SIMPLE_LAMP_COUNT  b'\x03'

/* GET_SOLENOID_COUNT - returns 1 byte for number of solenoids */
#define GET_SOLENOID_COUNT  b'\x04'

/* GET_SOUND_COUNT - returns 1 byte for umber of sounds - UNUSED */
#define GET_SOUND_COUNT  b'\x05'

/* GET_SEGMENT_DISPLAY_COUNT - returns 1 byte */
#define GET_SEGMENT_DISPLAY_COUNT  b'\x06'

/* GET_SEGMENT_DISPLAY_DETAILS - payload of 1 byte, returns 2 bytes */
#define GET_SEGMENT_DISPLAY_DETAILS  b'\x07'

/* GET_GAME_INFO - returns 1 byte */
#define GET_GAME_INFO  b'\x08'

/* GET_SWITCH_COUNT - returns 1 byte */
#define GET_SWITCH_COUNT  b'\x09'

/* GET_STATUS_OF_SIMPLE_LAMP - returns 1 byte */
#define GET_STATUS_OF_SIMPLE_LAMP  b'\x0A'

/* SET_SIMPLE_LAMP_TO_ON - no return */
#define SET_SIMPLE_LAMP_TO_ON  b'\x0B'

/* SET_SIMPLE_LAMP_TO_OFF - no return */
#define SET_SIMPLE_LAMP_TO_OFF  b'\x0C'

/* GET_STATUS_OF_SOLENOID - returns 1 byte */
#define GET_STATUS_OF_SOLENOID  b'\x14'

/* ENABLE_SOLENOID_FULL_POWER - no return */
#define ENABLE_SOLENOID_FULL_POWER  b'\x15'

/* DISABLE_SOLENOID - no return */
#define DISABLE_SOLENOID  b'\x16'

/* PULSE_SOLENOID - no return */
#define PULSE_SOLENOID  b'\x17'

/* SET_SOLENOID_PULSE_TIME - no return */
#define SET_SOLENOID_PULSE_TIME  b'\x18'

/* SET_SEGMENT_DISPLAY - UNUSED */
#define SET_SEGMENT_DISPLAY  b'\x1E'

/* GET_SWITCH_STATUS - returns 1 byte */
#define GET_SWITCH_STATUS  b'\x28'

/* NOTHING */
#define NOTHING  b'\x29'



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
