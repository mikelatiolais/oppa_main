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
};

/*
 * Serial protocol(based on OPP from MPF)
 */

#define GET_SER_NUM_CMD b'\x00'
#define RESET_CMD b'\x04'

 
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
