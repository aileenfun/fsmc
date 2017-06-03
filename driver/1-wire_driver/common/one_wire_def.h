#ifndef __ONE_WIRE_DEF_H__
#define __ONE_WIRE_DEF_H__

#include <stdint.h>

//ONE_WIRE ONE_WIRE error code
#define ONE_WIRE_ERROR_NO_ERROR                         0xAA

#define ONE_WIRE_ERROR_PRESENCE_DETECT_FAILED           0x01
#define ONE_WIRE_ERROR_TOUCH_BLOCK_FAILED				0x02
#define ONE_WIRE_ERROR_CRC8_CHECK_FAILED                0x03
#define ONE_WIRE_ERROR_CRC16_CHECK_FAILED               0x04

#define ONE_WIRE_CRC8_CHECK_RESULT                      0x00
#define ONE_WIRE_CRC16_CHECK_RESULT                     0xB001

#define ONE_WIRE_TRUE									1
#define ONE_WIRE_FALSE									0

struct one_wire_rom_id
{
    uint8_t family_code;
    uint8_t serial_num[6];
    uint8_t crc;
};

enum one_wire_speed_mode
{
    MODE_STANDARD,
    MODE_OVERDRIVE
};

typedef struct
{
    struct one_wire_rom_id rom_id;
    enum one_wire_speed_mode speed_mode;
    uint8_t crc8;
    uint16_t crc16;
    uint8_t error_code;
} one_wire_data_type;

#endif
