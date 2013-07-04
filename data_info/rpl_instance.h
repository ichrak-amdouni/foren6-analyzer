/* 
 * File:   rpl_instance.h
 * Author: am
 *
 * Created on June 21, 2013, 11:50 AM
 */

#ifndef RPL_INSTANCE_H
#define	RPL_INSTANCE_H

#include <stdbool.h>
#include "../uthash.h"

typedef struct di_rpl_instance_el *di_rpl_instance_hash_t;
typedef struct di_dodag_el *di_dodag_hash_t;

typedef enum tag_di_rpl_mop_e {
	RDMOP_NoDownwardRoute,
	RDMOP_NonStoring,
	RDMOP_StoringWithoutMulticast,
	RDMOP_StoringWithMulticast
} di_rpl_mop_e;

typedef struct di_rpl_instance {
	di_dodag_hash_t dodags;			//Via DIO, DAO
	di_rpl_mop_e mode_of_operation;	//Via DIO
	uint16_t instance_id;				//Via DIO, DAO
	
	void *user_data;
} di_rpl_instance_t;

typedef struct di_rpl_instance_el {
	di_rpl_instance_t *rpl_instance;
    UT_hash_handle hh;
} di_rpl_instance_el_t, *di_rpl_instance_hash_t;

di_rpl_instance_t *rpl_instance_get(di_rpl_instance_hash_t *hash, uint16_t instance_id, bool get_or_create);
bool rpl_instance_add(di_rpl_instance_hash_t *hash, di_rpl_instance_t *rpl_instance, bool overwrite_existing);
bool rpl_instance_remove(di_rpl_instance_hash_t *hash, uint16_t instance_id);

#endif	/* RPL_INSTANCE_H */

