/* 
 * File:   collected_data.h
 * Author: am
 *
 * Created on July 4, 2013, 10:16 AM
 */

#ifndef RPL_DATA_H
#define	RPL_DATA_H

#include "link.h"
#include "node.h"
#include "dodag.h"
#include "rpl_instance.h"
#include "hash_container.h"

typedef struct di_rpl_data {
//	di_node_hash_t nodes;
//	di_dodag_hash_t dodags;
//	di_rpl_instance_hash_t rpl_instances;
//	di_link_hash_t links;
	
	hash_container_ptr nodes;
	hash_container_ptr dodags;
	hash_container_ptr rpl_instances;
	hash_container_ptr links;
} di_rpl_data_t, rpl_collector_t;

void rpldata_init();
di_rpl_data_t *rpldata_get();
int rpldata_create_new_version();



#endif	/* RPL_DATA_H */

