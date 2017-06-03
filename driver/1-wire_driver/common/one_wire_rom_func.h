#ifndef __ONE_WIRE_ROM_FUNC_H__
#define __ONE_WIRE_ROM_FUNC_H__

#include "one_wire_def.h"

//ONE_WIRE ROM function commands
#define ONE_WIRE_ROM_COMMAND_READ_ROM                    0x33
#define ONE_WIRE_ROM_COMMAND_MATCH_ROM                   0x55
#define ONE_WIRE_ROM_COMMAND_SEARCH_ROM                  0xF0
#define ONE_WIRE_ROM_COMMAND_SKIP_ROM                    0xCC
#define ONE_WIRE_ROM_COMMAND_RESUME                      0xA5
#define ONE_WIRE_ROM_COMMAND_OVERDRIVE_SKIP_ROM          0x3C
#define ONE_WIRE_ROM_COMMAND_OVERDRIVE_MATCH_ROM         0x69

int8_t one_wire_read_rom(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_match_rom(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_search_rom(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_skip_rom(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_resume(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_overdrive_skip_rom(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_exit_overdrive_mode(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_overdrive_match_rom(uint8_t port, one_wire_data_type *one_wire_data);

#endif
