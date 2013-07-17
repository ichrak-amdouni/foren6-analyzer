#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

	bool has_changed;
	void *user_data;
} di_node_t;

size_t node_sizeof() {
	return sizeof(di_node_t);
}

void node_init(void *data, const void *key, size_t key_size) {
	di_node_t *node = (di_node_t*) data;
	di_node_key_t node_key = {*(di_node_ref_t*)key};

	assert(key_size == sizeof(di_node_ref_t));

	node->dodag.version = -1;
	node->is_custom_global_address = false;
	node->is_custom_local_address = false;
	node_set_key(node, &node_key);
	node->has_changed = true;
}

di_node_t *node_dup(di_node_t *node) {
	di_node_t *new_node;

	new_node = malloc(sizeof(di_node_t));
	memcpy(new_node, node, sizeof(di_node_t));
	new_node->routes = route_dup(&node->routes);

	return new_node;
}

void node_key_init(di_node_key_t *key, addr_wpan_t wpan_address, uint32_t version) {
	memset(key, 0, sizeof(di_node_key_t));

	key->ref.wpan_address = wpan_address;
}

void node_ref_init(di_node_ref_t *ref, addr_wpan_t wpan_address) {
	memset(ref, 0, sizeof(di_node_ref_t));

	ref->wpan_address = wpan_address;
}

void node_set_key(di_node_t *node, const di_node_key_t *key) {
	if(memcmp(&node->key, key, sizeof(di_node_key_t))) {
		node->key = *key;

		if(!node->is_custom_local_address) {
			node->local_address = addr_get_local_ip_from_mac64(node->key.ref.wpan_address);
		}

		node->has_changed = true;
	}
}

void node_set_local_ip(di_node_t *node, addr_ipv6_t address) {
	if(addr_compare_ip(&node->local_address, &address)) {
		node->local_address = address;
		node->has_changed = true;
	}
}

void node_set_global_ip(di_node_t *node, addr_ipv6_t address) {
	if(addr_compare_ip(&node->global_address, &address)) {
		node->global_address = address;
		node->has_changed = true;
	}
}

void node_add_route(di_node_t *node, const di_prefix_t *route_prefix, addr_wpan_t via_node) {
	bool route_already_existing = false;
	di_route_el_t *route_el = route_add(&node->routes, *route_prefix, false, &route_already_existing);

	if(route_already_existing == false || route_el->via_node != via_node) {
		route_el->via_node = via_node;
		node->has_changed = true;
	}
}

void node_del_route(di_node_t *node, const di_prefix_t *route_prefix) {
	if(route_remove(&node->routes, *route_prefix))
		node->has_changed = true;
}

void node_set_metric(di_node_t* node, const di_metric_t* metric) {
	if(node->metric.type != metric->type || node->metric.value != metric->value) {
		node->metric = *metric;
		node->has_changed = true;
	}
}

void node_set_rank(di_node_t *node, uint16_t rank) {
	if(node->rank != rank) {
		node->rank = rank;
		node->has_changed = true;
	}
}

void node_set_grounded(di_node_t *node, bool grounded) {
	if(node->grounded == grounded) {
		node->grounded = grounded;
		node->has_changed = true;
	}
}

void node_set_dodag(di_node_t *node, const di_dodag_ref_t *dodag_ref) {
	assert(dodag_ref->version >= 0);
	if(memcmp(&node->dodag, dodag_ref, sizeof(di_dodag_ref_t))) {
		node->dodag = *dodag_ref;
		node->has_changed = true;
	}
}

void node_set_user_data(di_node_t *node, void *data) {
	node->user_data = data;
}

void node_update_ip(di_node_t *node, const di_prefix_t *prefix) {
	if(!node->is_custom_global_address) {
		addr_ipv6_t ip = addr_get_global_ip_from_mac64(*prefix, node->key.ref.wpan_address);
		if(memcmp(&ip, &node->global_address, sizeof(addr_ipv6_t))) {
			node->global_address = ip;
			node->has_changed = true;
		}
	}
}

bool node_has_changed(di_node_t *node) {
	return node->has_changed;
}

void node_reset_changed(di_node_t *node) {
	node->has_changed = false;
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
