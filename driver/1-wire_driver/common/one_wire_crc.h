#ifndef __ONE_WIRE_CRC_H__
#define __ONE_WIRE_CRC_H__

#include "one_wire_def.h"

void set_crc16(one_wire_data_type *one_wire_data, uint16_t reset);
void set_crc8(one_wire_data_type *one_wire_data, uint8_t reset);
uint16_t calculate_crc16(one_wire_data_type *one_wire_data, uint16_t crc_data);
uint8_t calculate_crc8(one_wire_data_type *one_wire_data, uint8_t crc_data);

#endif
