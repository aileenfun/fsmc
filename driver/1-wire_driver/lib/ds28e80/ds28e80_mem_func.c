#include "one_wire_hal.h"
#include "one_wire_rom_func.h"
#include "ds28e80_mem_func.h"
#include "one_wire_crc.h"

#define DS28E80_MEM_BLOCK_PROGRAMING_TIME               20   //unit: ms

//DS28E80 memory function commands
#define DS28E80_MEM_COMMAND_WRITE_BLOCK                 0x55
#define DS28E80_MEM_COMMAND_READ_MEMORY                 0xF0
#define DS28E80_MEM_COMMAND_WRITE_PROTECT_BLOCK         0xC3
#define DS28E80_MEM_COMMAND_READ_BLOCK_PROTECTION       0xAA
#define DS28E80_MEM_COMMAND_READ_REMAINING_CYCLES       0xA5

//DS28E80 DS28E80 error code
#define DS28E80_ERROR_BLOCK_WRITE_PROTECTED             0x55
#define DS28E80_ERROR_BLOCK_WRITE_ACCESSES_EXHAUSTED    0x33
#define DS28E80_ERROR_INTERNAL_PROGRAMMING_ERROR        0xEE

#define DS28E80_ERROR_INVALID_BLOCK_NUMBER              0x05
#define DS28E80_ERROR_CS_BYTE_READ_ERROR                0x06
#define DS28E80_ERROR_READ_PROTECTION_ERROR             0x07
#define DS28E80_ERROR_READ_REMAINING_CYCLES_ERROR       0x08

#define DS28E80_TOTAL_BLOCK_NUM                         31
#define DS28E80_BLOCK_MAX_REMAINING_CYCLES              8

#define DS28E80_BLOCK_PROTECTED                         0xF0
#define DS28E80_BLOCK_UNPROTECTED                       0x0F

static void ds28e80_release_write_buffer(uint8_t port, one_wire_data_type *one_wire_data)
{
	one_wire_write_byte(port, one_wire_data, 0xFF);
}

int8_t ds28e80_write_block(uint8_t port, one_wire_data_type *one_wire_data,
                           uint8_t starting_block_num,
                           uint8_t num_of_blocks,
                           uint8_t *data)
{
    uint8_t i, j;

    if ((starting_block_num + num_of_blocks) > DS28E80_TOTAL_BLOCK_NUM)
    {
        one_wire_data->error_code = DS28E80_ERROR_INVALID_BLOCK_NUMBER;

        return -1;
    }

    set_crc16(one_wire_data, 0);

    one_wire_write_byte(port, one_wire_data, DS28E80_MEM_COMMAND_WRITE_BLOCK);
    calculate_crc16(one_wire_data, DS28E80_MEM_COMMAND_WRITE_BLOCK);

    one_wire_write_byte(port, one_wire_data, starting_block_num);
    calculate_crc16(one_wire_data, starting_block_num);

    for (i = 0; i < 2; i++)
    {
		calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
	}

    if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
    {
		one_wire_reset(port, one_wire_data);
		
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;        

        return -1;
    }

    for (i = 0; i < num_of_blocks; i++)
    {
        set_crc16(one_wire_data, 0);

        for (j = 0; j < 8; j++)
        {
            one_wire_write_byte(port, one_wire_data, data[j]);

            calculate_crc16(one_wire_data, data[j]);
        }

		for (j = 0; j < 2; j++)
		{
			calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
		}

        if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
        {
			one_wire_reset(port, one_wire_data);
			
            one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;            

            return -1;
        }

        ds28e80_release_write_buffer(port, one_wire_data);

        ONE_WIRE_DELAY_MS(DS28E80_MEM_BLOCK_PROGRAMING_TIME);

        one_wire_data->error_code = one_wire_read_byte(port, one_wire_data);

        if ((one_wire_data->error_code == DS28E80_ERROR_BLOCK_WRITE_PROTECTED) ||
                (one_wire_data->error_code == DS28E80_ERROR_BLOCK_WRITE_ACCESSES_EXHAUSTED) ||
                (one_wire_data->error_code == DS28E80_ERROR_INTERNAL_PROGRAMMING_ERROR))
        {
            one_wire_reset(port, one_wire_data);

            return -1;
        }
        else
        {
            if (((one_wire_data->error_code & 0x0F) == 0x0A) &&
                    (((one_wire_data->error_code & 0xF0) >> 8) < DS28E80_BLOCK_MAX_REMAINING_CYCLES))
            {
                one_wire_data->error_code = ONE_WIRE_ERROR_NO_ERROR;
            }
            else
            {
				one_wire_reset(port, one_wire_data);
				
                one_wire_data->error_code = DS28E80_ERROR_CS_BYTE_READ_ERROR;
                
                return -1;
            }
        }
    }

    one_wire_reset(port, one_wire_data);

    return 0;
}

int8_t ds28e80_read_memory(uint8_t port, 
							one_wire_data_type *one_wire_data,
                           uint8_t starting_block_num,
                           uint8_t num_of_blocks,
                           uint8_t *data)
{
    uint8_t i, j;

    if ((starting_block_num + num_of_blocks) > DS28E80_TOTAL_BLOCK_NUM)
    {
        one_wire_data->error_code = DS28E80_ERROR_INVALID_BLOCK_NUMBER;

        return -1;
    }

    set_crc16(one_wire_data, 0);

    one_wire_write_byte(port, one_wire_data, DS28E80_MEM_COMMAND_READ_MEMORY);
    calculate_crc16(one_wire_data, DS28E80_MEM_COMMAND_READ_MEMORY);

    one_wire_write_byte(port, one_wire_data, starting_block_num);
    calculate_crc16(one_wire_data, starting_block_num);

    for (i = 0; i < 2; i++)
    {
		calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
	}

    if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
    {
		one_wire_reset(port, one_wire_data);
		
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;        

        return -1;
    }

    for (i = 0; i < num_of_blocks; i++)
    {
        set_crc16(one_wire_data, 0);

        for (j = 0; j < 8; j++)
        {
            data[j] = one_wire_read_byte(port, one_wire_data);

            calculate_crc16(one_wire_data, data[j]);
        }

        for (j = 0; j < 2; j++)
		{
			calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
		}

        if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
        {
			one_wire_reset(port, one_wire_data);
			
            one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;            

            return -1;
        }
    }

    one_wire_data->error_code = ONE_WIRE_ERROR_NO_ERROR;

    one_wire_reset(port, one_wire_data);

    return 0;
}

int8_t ds28e80_write_protect_block(uint8_t port, one_wire_data_type *one_wire_data, uint8_t block_num)
{
	uint8_t i;
	
    if (block_num >= DS28E80_TOTAL_BLOCK_NUM)
    {
        one_wire_data->error_code = DS28E80_ERROR_INVALID_BLOCK_NUMBER;

        return -1;
    }

    set_crc16(one_wire_data, 0);

    one_wire_write_byte(port, one_wire_data, DS28E80_MEM_COMMAND_WRITE_PROTECT_BLOCK);
    calculate_crc16(one_wire_data, DS28E80_MEM_COMMAND_WRITE_PROTECT_BLOCK);

    one_wire_write_byte(port, one_wire_data, block_num);
    calculate_crc16(one_wire_data, block_num);

    for (i = 0; i < 2; i++)
    {
		calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
	}

    if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
    {
		one_wire_reset(port, one_wire_data);
		
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;        

        return -1;
    }

    ds28e80_release_write_buffer(port, one_wire_data);

    ONE_WIRE_DELAY_MS(DS28E80_MEM_BLOCK_PROGRAMING_TIME);

    one_wire_data->error_code = one_wire_read_byte(port, one_wire_data);

    if ((one_wire_data->error_code == DS28E80_ERROR_BLOCK_WRITE_PROTECTED) ||
            (one_wire_data->error_code == DS28E80_ERROR_INTERNAL_PROGRAMMING_ERROR))
    {
        one_wire_reset(port, one_wire_data);

        return -1;
    }
    else
    {
        if (one_wire_data->error_code != ONE_WIRE_ERROR_NO_ERROR)
        {
			one_wire_reset(port, one_wire_data);
			
            one_wire_data->error_code = DS28E80_ERROR_CS_BYTE_READ_ERROR;            

            return -1;
        }
    }

    one_wire_reset(port, one_wire_data);

    return 0;
}

int8_t ds28e80_read_block_protection(uint8_t port, 
									 one_wire_data_type *one_wire_data,
                                     uint8_t starting_block_num,
                                     uint8_t num_of_blocks,
                                     uint8_t *data)
{
    uint8_t i;

    if ((starting_block_num + num_of_blocks) > DS28E80_TOTAL_BLOCK_NUM)
    {
        one_wire_data->error_code = DS28E80_ERROR_INVALID_BLOCK_NUMBER;

        return -1;
    }

    set_crc16(one_wire_data, 0);

    one_wire_write_byte(port, one_wire_data, DS28E80_MEM_COMMAND_READ_BLOCK_PROTECTION);
    calculate_crc16(one_wire_data, DS28E80_MEM_COMMAND_READ_BLOCK_PROTECTION);

    one_wire_write_byte(port, one_wire_data, starting_block_num);
    calculate_crc16(one_wire_data, starting_block_num);
	
	for (i = 0; i < 2; i++)
    {
		calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
	}

    if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
    {
		one_wire_reset(port, one_wire_data);
		
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;        

        return -1;
    }

    for (i = 0; i < num_of_blocks; i++)
    {
        data[i] = one_wire_read_byte(port, one_wire_data);
        if (data[i] == DS28E80_BLOCK_UNPROTECTED)
        {
            data[i] = 0;
        }
        else if (data[i] == DS28E80_BLOCK_PROTECTED)
        {
            data[i] = 1;
        }
        else
        {
			one_wire_reset(port, one_wire_data);
			
            one_wire_data->error_code = DS28E80_ERROR_READ_PROTECTION_ERROR;            

            return -1;
        }
    }

    one_wire_data->error_code = ONE_WIRE_ERROR_NO_ERROR;

    one_wire_reset(port, one_wire_data);

    return 0;
}

int8_t ds28e80_read_remaining_cycles(uint8_t port, 
									 one_wire_data_type *one_wire_data,
                                     uint8_t starting_block_num,
                                     uint8_t num_of_blocks,
                                     uint8_t *data)
{
    uint8_t i;

    if ((starting_block_num + num_of_blocks) > DS28E80_TOTAL_BLOCK_NUM)
    {
        one_wire_data->error_code = DS28E80_ERROR_INVALID_BLOCK_NUMBER;

        return -1;
    }

    set_crc16(one_wire_data, 0);

    one_wire_write_byte(port, one_wire_data, DS28E80_MEM_COMMAND_READ_REMAINING_CYCLES);
    calculate_crc16(one_wire_data, DS28E80_MEM_COMMAND_READ_REMAINING_CYCLES);

    one_wire_write_byte(port, one_wire_data, starting_block_num);
    calculate_crc16(one_wire_data, starting_block_num);

    for (i = 0; i < 2; i++)
    {
		calculate_crc16(one_wire_data, one_wire_read_byte(port, one_wire_data));
	}

    if (one_wire_data->crc16 != ONE_WIRE_CRC16_CHECK_RESULT)
    {
		one_wire_reset(port, one_wire_data);
		
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC16_CHECK_FAILED;       

        return -1;
    }

    for (i = 0; i < num_of_blocks; i++)
    {
        data[i] = one_wire_read_byte(port, one_wire_data);
        if (data[i] > DS28E80_BLOCK_MAX_REMAINING_CYCLES)
        {
            one_wire_reset(port, one_wire_data);
			
			one_wire_data->error_code = DS28E80_ERROR_READ_REMAINING_CYCLES_ERROR;

            return -1;
        }
    }

    one_wire_data->error_code = ONE_WIRE_ERROR_NO_ERROR;

    one_wire_reset(port, one_wire_data);

    return 0;
}
