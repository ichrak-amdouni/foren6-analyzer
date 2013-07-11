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

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_rpl_data {
	hash_container_ptr nodes;
	hash_container_ptr dodags;
	hash_container_ptr rpl_instances;
	hash_container_ptr links;
} di_rpl_data_t;

void rpldata_init();
di_rpl_data_t *rpldata_get();

di_node_t *rpldata_get_node(const di_node_key_t *node_key);
di_dodag_t *rpldata_get_dodag(const di_dodag_key_t *dodag_key);
di_rpl_instance_t *rpldata_get_rpl_instance(const di_rpl_instance_key_t *rpl_instance_key);
di_link_t *rpldata_get_link(const di_link_key_t *link_key);

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_DATA_H */

