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

typedef struct di_rpl_instance {
	uint16_t instance_id;				//Via DIO, DAO
	uint32_t version;

	hash_container_ptr dodags;			//Via DIO, DAO
	di_rpl_mop_e mode_of_operation;	//Via DIO
	
	void *user_data;
} di_rpl_instance_t;

#endif	/* RPL_INSTANCE_H */

