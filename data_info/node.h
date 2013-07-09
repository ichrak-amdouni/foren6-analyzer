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

typedef struct {
	addr_wpan_t wpan_address;
} di_node_ref_t;

typedef struct di_node_key {
	di_node_ref_t ref;
	uint32_t version;
} di_node_key_t;

typedef struct di_node {
	di_node_key_t key;

	addr_ipv6_t local_address;
	addr_ipv6_t global_address;

	di_route_list_t routes;

	uint16_t rank;				//Via DIO
	di_metric_t metric;		//Usually ETX, via DIO with metric
	bool grounded;				//If true, can propagate packet to the root node.

	di_dodag_ref_t dodag;

	void *user_data;
} di_node_t;

void node_init(di_node_t *node);
di_node_t *node_dup(di_node_t *node);
void node_set_key(di_node_t *node, di_node_key_t *key);

#endif	/* NODE_H */

