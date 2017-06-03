#ifndef __ONE_WIRE_NET_H__
#define __ONE_WIRE_NET_H__

#include "one_wire_def.h"

typedef struct 
{
	uint8_t last_discrepancy;
	uint8_t last_device;
	uint8_t last_family_discrepancy;
}one_wire_search_state;

int8_t one_wire_search_first(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only);
int8_t one_wire_search_next(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only);
void one_wire_set_serial(uint8_t port, uint8_t *serialnum_buf, int8_t do_read);
void one_wire_family_search_setup(uint8_t port, uint8_t search_family);
void one_wire_skip_family(uint8_t port);
uint8_t one_wire_normal_search(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only);
uint8_t one_wire_family_search(uint8_t port, one_wire_data_type *one_wire_data, uint8_t search_family, uint8_t alarm_only);
int8_t one_wire_access(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_overdrive_access(uint8_t port, one_wire_data_type *one_wire_data);
int8_t one_wire_verify(uint8_t port, one_wire_data_type *one_wire_data, uint8_t alarm_only);

#endif
