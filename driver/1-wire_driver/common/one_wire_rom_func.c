// Include files
#include "one_wire_rom_func.h"
#include "one_wire_crc.h"
#include "one_wire_hal.h"

/*Read ROM*/
int8_t one_wire_read_rom(uint8_t port, one_wire_data_type *one_wire_data)
{
    uint8_t i;

    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_READ_ROM);

    one_wire_data->rom_id.family_code = one_wire_read_byte(port, one_wire_data);

    set_crc8(one_wire_data, 0);
	
	calculate_crc8(one_wire_data, one_wire_data->rom_id.family_code);

    for (i = 0; i < 6; i++)
    {
        one_wire_data->rom_id.serial_num[i] = one_wire_read_byte(port, one_wire_data);

        calculate_crc8(one_wire_data, one_wire_data->rom_id.serial_num[i]);
    }

    one_wire_data->rom_id.crc = one_wire_read_byte(port, one_wire_data);

    calculate_crc8(one_wire_data, one_wire_data->rom_id.crc);

    if (one_wire_data->crc8 != ONE_WIRE_CRC8_CHECK_RESULT)
    {
        one_wire_data->error_code = ONE_WIRE_ERROR_CRC8_CHECK_FAILED;
        return -1;
    }

    return 0;
}

/*Match ROM*/
int8_t one_wire_match_rom(uint8_t port, one_wire_data_type *one_wire_data)
{
    uint8_t i;
    uint8_t *p = (uint8_t *) & (one_wire_data->rom_id);
	
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_MATCH_ROM);

    for (i = 0; i < sizeof(one_wire_data->rom_id); i++)
    {
        one_wire_write_byte(port, one_wire_data, *p++);
    }
	
	return 0;
}

/*Search ROM*/
int8_t one_wire_search_rom(uint8_t port, one_wire_data_type *one_wire_data)
{
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_SEARCH_ROM);
	
	return 0;
}

/*Skip ROM*/
int8_t one_wire_skip_rom(uint8_t port, one_wire_data_type *one_wire_data)
{
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_SKIP_ROM);
	
	return 0;
}

/*Resume*/
int8_t one_wire_resume(uint8_t port, one_wire_data_type *one_wire_data)
{
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_RESUME);
	
	return 0;
}

/*Overdrive Skip ROM*/
int8_t one_wire_overdrive_skip_rom(uint8_t port, one_wire_data_type *one_wire_data)
{
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_OVERDRIVE_SKIP_ROM);

    one_wire_data->speed_mode = MODE_OVERDRIVE;
	
	return 0;
}

/*Exit Overdrive Skip ROM*/
int8_t one_wire_exit_overdrive_mode(uint8_t port, one_wire_data_type *one_wire_data)
{
	one_wire_data->speed_mode = MODE_STANDARD;
	
	if(one_wire_reset(port, one_wire_data) != 0)
	{
		one_wire_data->speed_mode = MODE_OVERDRIVE;
		return -1;
	}

    return 0;
}

/*Overdrive Match ROM*/
int8_t one_wire_overdrive_match_rom(uint8_t port, one_wire_data_type *one_wire_data)
{	
    uint8_t i;
    uint8_t *p = (uint8_t *) & (one_wire_data->rom_id);
	
    one_wire_write_byte(port, one_wire_data, ONE_WIRE_ROM_COMMAND_OVERDRIVE_MATCH_ROM);

    one_wire_data->speed_mode = MODE_OVERDRIVE;

    for (i = 0; i < sizeof(one_wire_data->rom_id); i++)
    {
        one_wire_write_byte(port, one_wire_data, *p++);
    }
	
	return 0;
}
