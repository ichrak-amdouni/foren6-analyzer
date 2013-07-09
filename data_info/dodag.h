/* 
 * File:   dodag.h
 * Author: am
 *
 * Created on June 20, 2013, 5:04 PM
 */

#ifndef DODAG_H
#define	DODAG_H

#include "address.h"
#include "rpl_instance.h"
#include "hash_container.h"

typedef enum tag_di_objective_function_e {
	ROF_ETX = 1
} di_objective_function_e;

typedef struct di_dodag_config {
	uint16_t auth_enabled;
	uint8_t path_control_size;
	uint8_t dio_interval_min;
	uint8_t dio_interval_max;
	uint8_t dio_redundancy_constant;
	uint16_t max_rank_inc;
	uint16_t min_hop_rank_inc;
	uint8_t default_lifetime;
	uint16_t lifetime_unit;
	di_objective_function_e objective_function;
} di_dodag_config_t;

typedef struct di_dodag_key {
	addr_ipv6_t dodagid;				//Via DIO, DAO
	uint16_t version;					//Via DIO
} di_dodag_key_t;

typedef struct di_dodag {
	di_dodag_key_t dodag_key;				//Via DIO & DAO for dodagid and via DIO for version
	uint32_t version;
	
	//Configuration
	di_dodag_config_t config;				//Via DIO config option

	di_prefix_t prefix;						//Via DIO prefix option

	int32_t rpl_instance;		//Via DIO, DAO
	
	//Nodes
	hash_container_ptr nodes;					//Via DIO, sometimes DAO
	
	void *user_data;
} di_dodag_t;

#endif	/* DODAG_H */

