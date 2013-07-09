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
#include "node.h"

typedef struct di_link_ref {
	di_node_ref_t child;
	di_node_ref_t parent;
} di_link_ref_t;

typedef struct di_link_key {
	di_link_ref_t ref;
	uint32_t version;
} di_link_key_t;


typedef struct di_link {
	di_link_key_t key;

	di_metric_t metric;
	time_t last_update;		//TX only
	time_t expiration_time;
	uint32_t packet_count;	//TX only
	
	void *user_data;
} di_link_t;

void link_init(di_link_t *link);
bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count);
di_link_t *link_dup(di_link_t *link);

#endif	/* LINK_H */

