#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"
#include "route.h"

typedef struct di_node {
	di_node_key_t key;

	bool is_custom_local_address;
	addr_ipv6_t local_address;

	bool is_custom_global_address;
	addr_ipv6_t global_address;

	di_route_list_t routes;

	uint16_t rank;				//Via DIO
	di_metric_t metric;		//Usually ETX, via DIO with metric
	bool grounded;				//If true, can propagate packet to the root node.

	di_dodag_ref_t dodag;

	void *user_data;
} di_node_t;

size_t node_sizeof() {
	return sizeof(di_node_t);
}

void node_init(void *data, void *key, size_t key_size) {
	di_node_t *node = (di_node_t*) data;

	assert(key_size == sizeof(di_node_key_t));

	node->dodag.version = -1;
	node->is_custom_global_address = false;
	node->is_custom_local_address = false;
	node_set_key(node, key);
}

di_node_t *node_dup(di_node_t *node) {
	di_node_t *new_node;

	new_node = malloc(sizeof(di_node_t));
	memcpy(new_node, node, sizeof(di_node_t));
	new_node->routes = route_dup(&node->routes);

	return new_node;
}


void node_set_key(di_node_t *node, const di_node_key_t *key) {
	node->key = *key;

	if(!node->is_custom_local_address) {
		node->local_address = addr_get_local_ip_from_mac64(node->key.ref.wpan_address);
	}
}

void node_set_local_ip(di_node_t *node, addr_ipv6_t address) {
	node->local_address = address;
}

void node_set_global_ip(di_node_t *node, addr_ipv6_t address) {
	node->global_address = address;
}

void node_add_route(di_node_t *node, const di_prefix_t *route_prefix, addr_wpan_t via_node) {
	di_route_el_t *route_el = route_add(&node->routes, *route_prefix, false);
	route_el->via_node = via_node;
}

void node_del_route(di_node_t *node, const di_prefix_t *route_prefix) {
	route_remove(&node->routes, *route_prefix);
}

void node_set_metric(di_node_t* node, const di_metric_t* metric) {
	node->metric = *metric;
}

void node_set_rank(di_node_t *node, uint16_t rank) {
	node->rank = rank;
}

void node_set_grounded(di_node_t *node, bool grounded) {
	node->grounded = grounded;
}

void node_set_dodag(di_node_t *node, const di_dodag_ref_t *dodag_ref) {
	assert(dodag_ref->version >= 0);
	node->dodag = *dodag_ref;
}

void node_set_user_data(di_node_t *node, void *data) {
	node->user_data = data;
}

void node_update_ip(di_node_t *node, const di_prefix_t *prefix) {
	if(!node->is_custom_global_address) {
		node->global_address = addr_get_global_ip_from_mac64(*prefix, node->key.ref.wpan_address);
	}
}


const di_node_key_t *node_get_key(const di_node_t *node) {
	return &node->key;
}

addr_wpan_t node_get_mac64(const di_node_t *node) {
	return node->key.ref.wpan_address;
}

const addr_ipv6_t* node_get_local_ip(const di_node_t *node) {
	return &node->local_address;
}

const addr_ipv6_t* node_get_global_ip(const di_node_t *node) {
	return &node->global_address;
}

const di_route_list_t* node_get_routes(const di_node_t *node) {
	return &node->routes;
}

const di_metric_t *node_get_metric(const di_node_t* node) {
	return &node->metric;
}

uint16_t node_get_rank(const di_node_t *node) {
	return node->rank;
}

bool node_get_grounded(const di_node_t *node) {
	return node->grounded;
}

const di_dodag_ref_t * node_get_dodag(const di_node_t *node) {
	if(node->dodag.version >= 0)
		return &node->dodag;

	return NULL;
}

void *node_get_user_data(const di_node_t *node) {
	return node->user_data;
}
