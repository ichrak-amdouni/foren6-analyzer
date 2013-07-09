/* 
 * File:   rpl_instance.h
 * Author: am
 *
 * Created on June 21, 2013, 11:50 AM
 */

#ifndef RPL_INSTANCE_H
#define	RPL_INSTANCE_H

#include <stdbool.h>
#include <stdint.h>
#include "hash_container.h"

typedef enum tag_di_rpl_mop_e {
	RDMOP_NoDownwardRoute,
	RDMOP_NonStoring,
	RDMOP_StoringWithoutMulticast,
	RDMOP_StoringWithMulticast
} di_rpl_mop_e;

typedef struct {
	int16_t rpl_instance;				//Via DIO, DAO
} di_rpl_instance_ref_t;

typedef struct di_rpl_instance_key {
	di_rpl_instance_ref_t ref;
	uint32_t version;
} di_rpl_instance_key_t;

typedef struct di_rpl_instance {
	di_rpl_instance_key_t key;

	hash_container_ptr dodags;			//Via DIO, DAO
	di_rpl_mop_e mode_of_operation;	//Via DIO
	
	void *user_data;
} di_rpl_instance_t;

void rpl_instance_init(di_rpl_instance_t* instance);
di_rpl_instance_t* rpl_instance_dup(di_rpl_instance_t* rpl_instance);

#endif	/* RPL_INSTANCE_H */

