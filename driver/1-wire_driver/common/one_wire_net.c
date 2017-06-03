// Include files
#include "one_wire_net.h"
#include "one_wire_crc.h"
#include "one_wire_hal.h"
#include "one_wire_rom_func.h"

// exportable functions defined in ownet.c

// global variables for this module to hold search state information
static one_wire_search_state search_state[ONE_WIRE_PORT_NUM];
static uint8_t serial[8][ONE_WIRE_PORT_NUM];

//--------------------------------------------------------------------------
// The 'one_wire_search_first' finds the first device on the 1-Wire Net  This function
// contains one parameter 'alarm_only'.  When
// 'alarm_only' is ONE_WIRE_TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
int8_t one_wire_search_first(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only)
{
	// reset the search state
	search_state[port].last_discrepancy = 0;
	search_state[port].last_device = ONE_WIRE_FALSE;
	search_state[port].last_family_discrepancy = 0;	

	return one_wire_search_next(port, one_wire_data, alarm_only);
}

//--------------------------------------------------------------------------
// The 'one_wire_search_next' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'owFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is ONE_WIRE_TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
int8_t one_wire_search_next(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only)
{
	uint8_t bit_test, search_direction, bit_number;
	uint8_t last_zero, serial_byte_number;
	uint8_t serial_byte_mask;
	int8_t next_result;
	

   // initialize for search
   bit_number = 1;
   last_zero = 0;
   serial_byte_number = 0;
   serial_byte_mask = 1;
   next_result = -1;
   set_crc8(one_wire_data,0);

   // if the last call was not the last one
   if (search_state[port].last_device == 0)
   {
		// reset the 1-wire
		// if there are no parts on 1-wire, return ONE_WIRE_FALSE
		if (one_wire_reset(port, one_wire_data) != 0)
		{
			// reset the search state
			search_state[port].last_discrepancy = 0;
			search_state[port].last_device = ONE_WIRE_FALSE;
			search_state[port].last_family_discrepancy = 0;
			one_wire_data->error_code = ONE_WIRE_ERROR_PRESENCE_DETECT_FAILED;
			return -1;
		}

		// If finding alarming devices issue a different command
		if (alarm_only == ONE_WIRE_FALSE)
		{
			one_wire_search_rom(port, one_wire_data);  // issue the search command
		}
//		else
//		{
//			one_wire_search_rom(one_wire_data);  // issue the alarming search command
//		}

		// loop to do the search
		do
		{
			// read a bit and its compliment
			bit_test = one_wire_read_bit(port, one_wire_data) << 1;
			bit_test |= one_wire_read_bit(port, one_wire_data);

			// check for no devices on 1-wire
			if (bit_test == 3)
			{
				next_result = -1;
				break;
			}
			else
			{
				// all devices coupled have 0 or 1
				if (bit_test > 0)
				{
					search_direction = !(bit_test & 0x01);  // bit write value for search
				}
				else
				{
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (bit_number < search_state[port].last_discrepancy)
					{
						search_direction = (((serial[port][serial_byte_number] & serial_byte_mask) > 0) ? 1 : 0);
					}
					else
					{
						// if equal to last pick 1, if not then pick 0
						search_direction = ((bit_number == search_state[port].last_discrepancy) ? 1 : 0);
					}

					// if 0 was picked then record its position in LastZero
					if (search_direction == 0)
					{
						last_zero = bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9)
						{
							search_state[port].last_family_discrepancy = last_zero;
						}
					}
				}

				// set or clear the bit in the serial byte serial_byte_number
				// with mask serial_byte_mask
				if (search_direction == 1)
				{
					serial[port][serial_byte_number] |= serial_byte_mask;
				}
				else
				{
					serial[port][serial_byte_number] &= ~serial_byte_mask;
				}

				// serial number search direction write bit
				one_wire_write_bit(port, one_wire_data, search_direction);

				// increment the byte counter bit_number
				// and shift the mask serial_byte_mask
				bit_number++;
				serial_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum[portnum] byte serial_byte_number
				// and reset mask
				if (serial_byte_mask == 0)
				{
//					// The below has been added to accomidate the valid CRC with the
//					// possible changing serial number values.
//					if (((serial[0] & 0x7F) == 0x1C) && (serial_byte_number == 1))
//					{
//						one_wire_data->crc8 = calculate_crc8(one_wire_data, 0x7F);
//					}
//					else
//					{
					one_wire_data->crc8 = calculate_crc8(one_wire_data, serial[port][serial_byte_number]);  // accumulate the CRC
//					}

					serial_byte_number++;
					serial_byte_mask = 1;
				}
			}
		}
		while(serial_byte_number < 8);  // loop until through all SerialNum[portnum] bytes 0-7

		// if the search was successful then
		if ((bit_number > 64) && (one_wire_data->crc8 == 0))
//		if (!((bit_number < 65) || one_wire_data->crc8))
		{
			// search successful so set LastDiscrepancy[portnum],LastDevice[portnum],next_result
			search_state[port].last_discrepancy = last_zero;
			search_state[port].last_device = (search_state[port].last_discrepancy == 0);
			next_result = 0;
			
			one_wire_set_serial(port, (uint8_t*)&(one_wire_data->rom_id), 1);
		}
   }

   // if no device found then reset counters so next 'next' will be
   // like a first
   if ((next_result == -1) || (serial[port][0] == 0))
   {
      search_state[port].last_discrepancy = 0;
      search_state[port].last_device = ONE_WIRE_FALSE;
      search_state[port].last_family_discrepancy = 0;
      next_result = -1;
   }

   return next_result;
}

//--------------------------------------------------------------------------
// The 'one_wire_set_serial' function either reads or sets the serial buffer
// that is used in the search functions 'one_wire_search_first' and 'one_wire_search_next'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is ONE_WIRE_TRUE (1) if the operation is to read and ONE_WIRE_FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = ONE_WIRE_FALSE (0) and buffer to get the serial
//                   number when do_read = ONE_WIRE_TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void one_wire_set_serial(uint8_t port, uint8_t *serialnum_buf, int8_t do_read)
{
	uint8_t i;

	// read the internal buffer and place in 'serialnum_buf'
	if (do_read)
	{
		for (i = 0; i < 8; i++)
		{
			serialnum_buf[i] = serial[port][i];
		}
	}
	// set the internal buffer from the data in 'serialnum_buf'
	else
	{
		for (i = 0; i < 8; i++)
		{
			serial[port][i] = serialnum_buf[i];
		}
	}
}

//--------------------------------------------------------------------------
// Setup the search algorithm to find a certain family of devices
// the next time a search function is called 'one_wire_search_next'.
//
// 'search_family' - family code type to set the search algorithm to find
//                   next.
//
void one_wire_family_search_setup(uint8_t port, uint8_t search_family)
{
	uint8_t i;

	// set the search state to find SearchFamily type devices
	serial[port][0] = search_family;
	for (i = 1; i < 8; i++)
	{
		serial[port][i] = 0;
	}
	search_state[port].last_discrepancy = 64;
	search_state[port].last_device = ONE_WIRE_FALSE;
}

//--------------------------------------------------------------------------
// Set the current search state to skip the current family code.
//
void one_wire_skip_family(uint8_t port)
{
	// set the Last discrepancy to last family discrepancy
	search_state[port].last_discrepancy = search_state[port].last_family_discrepancy;
	search_state[port].last_family_discrepancy = 0;

	// check for end of list
	if (search_state[port].last_discrepancy == 0)
	{
		search_state[port].last_device = ONE_WIRE_TRUE;
	}
}

//--------------------------------------------------------------------------
// Normal search routine.
//
// Return: Device number that are exist on the one-wire bus.
//
uint8_t one_wire_normal_search(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only)
{
	uint8_t device_num = 0;
	uint8_t serial_num[8] = {0};
	int8_t result;
	
	one_wire_data->speed_mode = MODE_STANDARD;
	
	one_wire_set_serial(port, serial_num, 0);	
	
	result = one_wire_search_first(port, one_wire_data, alarm_only);
	
	if(result == 0)
	{
		device_num++;
		do
		{
			one_wire_data[device_num].speed_mode = MODE_STANDARD;
			
			result = one_wire_search_next(port, &one_wire_data[device_num], alarm_only);
			if(result == 0)
			{
				device_num++;
			}
		}while((result == 0) && (device_num < ONE_WIRE_NET_MAX_DEVICE_NUM));
	}
	
	return device_num;
}

//--------------------------------------------------------------------------
// Select family search routine.
//
// Return: Device number that are exist on the one-wire bus.
//
uint8_t one_wire_family_search(uint8_t port, one_wire_data_type *one_wire_data, uint8_t search_family, uint8_t alarm_only)
{
	uint8_t device_num = 0;
	int8_t result;

	one_wire_data->speed_mode = MODE_STANDARD;
	
	one_wire_family_search_setup(port, search_family);
	
	do
	{
		one_wire_data[device_num].speed_mode = MODE_STANDARD;
		
		result = one_wire_search_next(port, &one_wire_data[device_num], alarm_only);
		if(result == 0)
		{
			device_num++;
		}
	}while((result == 0) && (device_num < ONE_WIRE_NET_MAX_DEVICE_NUM));
	
	return device_num;
}

//--------------------------------------------------------------------------
// The 'owAccess' function resets the 1-Wire and sends a MATCH Serial
// Number command followed by the current SerialNum code. After this
// function is complete the 1-Wire device is ready to accept device-specific
// commands.
//
// Returns:   0 : reset indicates present and device is ready
//                       for commands.
//            -1: reset does not indicate presence or echos 'writes'
//                       are not correct.
//
int8_t one_wire_access(uint8_t port, one_wire_data_type *one_wire_data)
{
   // reset the 1-wire
   if (one_wire_reset(port, one_wire_data) != 0)
   {
	   one_wire_data->error_code = ONE_WIRE_ERROR_PRESENCE_DETECT_FAILED;
	   return -1;
   }
   else
   {
	   if(one_wire_match_rom(port, one_wire_data) != 0)
	   {
		   return -1;
	   }     
   }

   return 0;
}

//----------------------------------------------------------------------
// The function 'owVerify' verifies that the current device
// is in contact with the 1-Wire Net.
// Using the find alarm command 0xEC will verify that the device
// is in contact with the 1-Wire Net and is in an 'alarm' state.
//
// 'alarm_only'  - ONE_WIRE_TRUE (1) the find alarm command 0xEC
//                 is sent instead of the normal search
//                 command 0xF0.
//
// Returns:   0 : when the 1-Wire device was verified
//                to be on the 1-Wire Net
//                with alarm_only == ONE_WIRE_FALSE
//                or verified to be on the 1-Wire Net
//                AND in an alarm state when
//                alarm_only == ONE_WIRE_TRUE.
//            -1: the 1-Wire device was not on the
//                1-Wire Net or if alarm_only
//                == ONE_WIRE_TRUE, the device may be on the
//                1-Wire Net but in a non-alarm state.
//
int8_t one_wire_verify(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only)
{
	int8_t result;
	one_wire_data_type one_wire;
	
	search_state[port].last_discrepancy = 64;
	search_state[port].last_device = ONE_WIRE_FALSE;
	
	one_wire_set_serial(port, (uint8_t*)&one_wire_data->rom_id, alarm_only);
	
	one_wire.speed_mode = MODE_STANDARD;
	
	result = one_wire_search_next(port, &one_wire, 0);
	
	if(result == 0)
	{
		uint8_t i;
		uint8_t *temp = (uint8_t*)&one_wire_data->rom_id;
		uint8_t *temp1 = (uint8_t *)&one_wire.rom_id;
		
		for(i = 0; i < sizeof(one_wire.rom_id); i++)
		{
			if(temp[i] != temp1[i])
			{
				result = -1;
			}
		}
	}
	
	return result;
	
}

//----------------------------------------------------------------------
// Perform a overdrive MATCH command to select the 1-Wire device with
// the address in the ID data register.
//
// Returns:  0: If the device is present on the 1-Wire Net and
//                 can do overdrive then the device is selected.
//           -1: Device is not present or not capable of overdrive.
//
int8_t one_wire_overdrive_access(uint8_t port, one_wire_data_type *one_wire_data)
{
	
   // reset the 1-wire
   if (one_wire_reset(port, one_wire_data) != 0)
   {
	   one_wire_data->error_code = ONE_WIRE_ERROR_PRESENCE_DETECT_FAILED;
	   return -1;
   }
   else
   {
	   if(one_wire_overdrive_match_rom(port, one_wire_data) != 0)
	   {
		   one_wire_data->speed_mode = MODE_STANDARD;
		   return -1;
	   }     
   }

   return 0;
}
