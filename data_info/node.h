/* 
 * File:   node.h
 * Author: am
 *
 * Created on June 20, 2013, 4:12 PM
 */

#ifndef NODE_H
#define	NODE_H

#include "metric.h"
#include "address.h"
#include "route.h"
#include "dodag.h"

typedef struct di_node {
	addr_wpan_t wpan_address;
	uint32_t version;
	
	addr_ipv6_t local_address;
	addr_ipv6_t global_address;
	
	di_route_list_t routes;
	
	uint16_t rank;				//Via DIO
	di_metric_t metric;		//Usually ETX, via DIO with metric
	bool grounded;				//If true, can propagate packet to the root node.
	
	di_dodag_key_t dodag;
	
	void *user_data;
} di_node_t;

#endif	/* NODE_H */

