/* 
 * File:   link.h
 * Author: am
 *
 * Created on June 21, 2013, 10:42 AM
 */

#ifndef LINK_H
#define	LINK_H

#include <stdbool.h>
#include <stdint.h>
#include "metric.h"
#include "address.h"
#include "../uthash.h"


typedef struct di_node di_node_t;

typedef struct di_link_addr_pair {
	di_node_t *child;
	di_node_t *parent;
} di_link_addr_pair_t;

typedef struct di_link {
	di_link_addr_pair_t key;

	di_metric_t metric;
	time_t last_update;		//TX only
	time_t expiration_time;
	uint32_t packet_count;	//TX only
	
	void *user_data;
} di_link_t;

typedef struct di_link_el {
	di_link_t *link;
	
    UT_hash_handle hh;
} di_link_el_t, *di_link_hash_t;

di_link_t *link_hash_get(di_link_hash_t *hash, di_node_t *child_node, di_node_t *parent_node, bool get_or_create);
bool link_hash_del(di_link_hash_t *hash, di_node_t *child_node, di_node_t *parent_node);
bool link_hash_remove_all_outdated(di_link_hash_t *hash);
bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count);

#endif	/* LINK_H */

