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

typedef struct di_link_addr_pair {
	addr_wpan_t child;
	addr_wpan_t parent;
} di_link_addr_pair_t;

typedef struct di_link {
	di_link_addr_pair_t key;
	uint32_t version;

	di_metric_t metric;
	time_t last_update;		//TX only
	time_t expiration_time;
	uint32_t packet_count;	//TX only
	
	void *user_data;
} di_link_t;

bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count);

#endif	/* LINK_H */

