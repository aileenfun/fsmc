#ifndef __DS28E80_MEM_FUNC_H__
#define __DS28E80_MEM_FUNC_H__

#include "one_wire_def.h"

int8_t ds28e80_write_block(uint8_t port, 
						   one_wire_data_type *one_wire_data,
                           uint8_t starting_block_num,
                           uint8_t num_of_blocks,
                           uint8_t *data);
int8_t ds28e80_read_memory(uint8_t port,
						   one_wire_data_type *one_wire_data,
                           uint8_t starting_block_num,
                           uint8_t num_of_blocks,
                           uint8_t *data);
int8_t ds28e80_write_protect_block(uint8_t port, 
								   one_wire_data_type *one_wire_data, uint8_t block_num);
int8_t ds28e80_read_block_protection(uint8_t port, 
									 one_wire_data_type *one_wire_data,
                                     uint8_t starting_block_num,
                                     uint8_t num_of_blocks,
                                     uint8_t *data);
int8_t ds28e80_read_remaining_cycles(uint8_t port, 
									 one_wire_data_type *one_wire_data,
                                     uint8_t starting_block_num,
                                     uint8_t num_of_blocks,
                                     uint8_t *data);

#endif
