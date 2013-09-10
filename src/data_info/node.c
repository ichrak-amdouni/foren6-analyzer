#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "node.h"
#include "route.h"
#include "../data_collector/rpl_event_callbacks.h"
#include "rpl_data.h"

struct di_node {
	di_node_key_t key;
	uint16_t simple_id;

	bool is_custom_local_address;
	addr_ipv6_t local_address;

	bool is_custom_global_address;
	addr_ipv6_t global_address;

	di_route_list_t routes;

	uint16_t rank;				//Via DIO
	di_metric_t metric;		//Usually ETX, via DIO with metric
	bool grounded;				//If true, can propagate packet to the root node.
	int latest_dao_sequence;
	int latest_dtsn;

	di_dodag_ref_t dodag;

	bool has_changed;
	void *user_data;

	//statistics
	int packet_count;
	double max_dao_interval;  //maximum interval seen between 2 DAO packets
	double max_dio_interval;  //maximum interval seen between 2 DIO packets
	int upward_rank_errors;   //incremented when data trafic goes from a node to it's parent in the case of the parent has a greater rank than the child
	int downward_rank_errors; //incremented when data trafic goes from a node to a child where the child have a smaller rank than the parent
	int route_loop_errors;    //incremented when a node choose a parent that is in it's routing table

	int ip_mismatch_errors;   //incremented when a DAO message is sent to a node with the wrong IP<=>WPAN addresses association
	int dodag_version_decrease_errors;  //incremented when a DIO message contain a dodag version smaller than the known version
	int dodag_mismatch_errors; //incremented when a DAO is sent to a parent with the dodagid in the DAO packet different from the parent's dodag
};

static uint16_t last_simple_id = 0;
static void node_set_changed(di_node_t *node);
static void node_update_old_field(const di_node_t *node, int field_offset, int field_size);

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
	node->packet_count = 0;
	node->max_dao_interval = 0;
	node->latest_dao_sequence = 0;
	node->latest_dtsn = 0;
	node->has_changed = true;

	last_simple_id++;
	node->simple_id = last_simple_id;

	node_set_key(node, &node_key);

	rpl_event_node(node, RET_Created);
}

void node_destroy(void *data) {
	di_node_t *node = (di_node_t*) data;

	route_destroy(&node->routes);
}

di_node_t *node_dup(const di_node_t *node) {
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

		node_set_changed(node);
	}
}

void node_set_local_ip(di_node_t *node, addr_ipv6_t address) {
	if(addr_compare_ip(&node->local_address, &address)) {
		node->local_address = address;
		node->is_custom_local_address = true;
		node_set_changed(node);
	}
}

void node_set_global_ip(di_node_t *node, addr_ipv6_t address) {
	if(addr_compare_ip(&node->global_address, &address)) {
		node->global_address = address;
		node->is_custom_global_address = true;
		node_set_changed(node);
	}
}

void node_add_route(di_node_t *node, const di_prefix_t *route_prefix, addr_wpan_t via_node) {
	bool route_already_existing = false;
	route_add(&node->routes, *route_prefix, via_node, false, &route_already_existing);

	if(route_already_existing == false) {
		node_set_changed(node);
	}
}

void node_del_route(di_node_t *node, const di_prefix_t *route_prefix, addr_wpan_t via_node) {
	if(route_remove(&node->routes, *route_prefix, via_node))
		node_set_changed(node);
}

void node_set_metric(di_node_t* node, const di_metric_t* metric) {
	if(node->metric.type != metric->type || node->metric.value != metric->value) {
		node->metric = *metric;
		node_set_changed(node);
	}
}

void node_set_rank(di_node_t *node, uint16_t rank) {
	if(node->rank != rank) {
		node->rank = rank;
		node_set_changed(node);
	}
}

void node_set_grounded(di_node_t *node, bool grounded) {
	if(node->grounded == grounded) {
		node->grounded = grounded;
		node_set_changed(node);
	}
}

void node_set_dodag(di_node_t *node, const di_dodag_ref_t *dodag_ref) {
	if(memcmp(&node->dodag, dodag_ref, sizeof(di_dodag_ref_t))) {
		node->dodag = *dodag_ref;
		node_set_changed(node);
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
			node_set_changed(node);
		}
	}
}

void node_add_packet_count(di_node_t *node, int count) {
	node->packet_count += count;

	node_update_old_field(node, offsetof(di_node_t, packet_count), sizeof(node->packet_count));
}

static void node_set_changed(di_node_t *node) {
	if(node->has_changed == false)
		rpl_event_node(node, RET_Updated);
	node->has_changed = true;
}

static void node_update_old_field(const di_node_t *node, int field_offset, int field_size) {
	di_node_t **versionned_node_ptr;
	int version = rpldata_get_wsn_last_version();
	if(version) {
		hash_container_ptr container = rpldata_get_nodes(version);
		if(container) {
			versionned_node_ptr = (di_node_t**)hash_value(container, hash_key_make(node->key.ref), HVM_FailIfNonExistant, NULL);
			if(versionned_node_ptr && *versionned_node_ptr != node)
				memcpy((char*)(*versionned_node_ptr) + field_offset, (char*)node + field_offset, field_size);
		}
	}
}

bool node_has_changed(di_node_t *node) {
	return node->has_changed;
}

void node_reset_changed(di_node_t *node) {
	node->has_changed = false;
}

void node_set_dtsn(di_node_t *node, int dtsn) {
	node->latest_dtsn = dtsn;
	node_update_old_field(node, offsetof(di_node_t, latest_dtsn), sizeof(node->latest_dtsn));
}

void node_set_dao_seq(di_node_t *node, int dao_seq) {
	node->latest_dao_sequence = dao_seq;
	node_update_old_field(node, offsetof(di_node_t, latest_dao_sequence), sizeof(node->latest_dao_sequence));
}

void node_update_dao_interval(di_node_t *node, double timestamp) {
	static double last_dao_timestamp = 0;

	if(last_dao_timestamp) {
		double interval = timestamp - last_dao_timestamp;
		if(!node->max_dao_interval || (node->max_dao_interval < interval)) {
			node->max_dao_interval = interval;
			node_update_old_field(node, offsetof(di_node_t, max_dao_interval), sizeof(node->max_dao_interval));
		}
	}

	last_dao_timestamp = timestamp;
}

void node_update_dio_interval(di_node_t *node, double timestamp) {
	static double last_dio_timestamp = 0;

	if(last_dio_timestamp) {
		double interval = timestamp - last_dio_timestamp;
		if(!node->max_dio_interval || (node->max_dio_interval < interval)) {
			node->max_dio_interval = interval;
			node_update_old_field(node, offsetof(di_node_t, max_dio_interval), sizeof(node->max_dio_interval));
		}
	}

	last_dio_timestamp = timestamp;
}

void node_add_upward_error(di_node_t *node) {
	node->upward_rank_errors++;
	node_set_changed(node);
}

void node_add_downward_error(di_node_t *node) {
	node->downward_rank_errors++;
	node_set_changed(node);
}

void node_add_route_error(di_node_t *node) {
	node->route_loop_errors++;
	node_set_changed(node);
}

void node_add_dodag_version_error(di_node_t *node) {
	node->dodag_version_decrease_errors++;
	node_set_changed(node);
}

void node_add_ip_mismatch_error(di_node_t *node) {
	node->ip_mismatch_errors++;
	node_set_changed(node);
}

void node_add_dodag_mismatch_error(di_node_t *node) {
	node->dodag_mismatch_errors++;
	node_set_changed(node);
}


const di_node_key_t *node_get_key(const di_node_t *node) {
	return &node->key;
}

addr_wpan_t node_get_mac64(const di_node_t *node) {
	return node->key.ref.wpan_address;
}

uint16_t node_get_simple_id(const di_node_t *node) {
	return node->simple_id;
}

const addr_ipv6_t* node_get_local_ip(const di_node_t *node) {
	return &node->local_address;
}

const addr_ipv6_t* node_get_global_ip(const di_node_t *node) {
	return &node->global_address;
}

di_route_list_t node_get_routes(const di_node_t *node) {
	return node->routes;
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

int node_get_packet_count(const di_node_t *node) {
	return node->packet_count;
}

int node_get_dtsn(const di_node_t *node) {
	return node->latest_dtsn;
}

int node_get_dao_seq(const di_node_t *node) {
	return node->latest_dao_sequence;
}

double node_get_max_dao_interval(const di_node_t *node) {
	return node->max_dao_interval;
}

double node_get_max_dio_interval(const di_node_t *node) {
	return node->max_dio_interval;
}

int node_get_upward_error_count(const di_node_t *node) {
	return node->upward_rank_errors;
}

int node_get_downward_error_count(const di_node_t *node) {
	return node->downward_rank_errors;
}

int node_get_route_error_count(const di_node_t *node) {
	return node->route_loop_errors;
}

int node_get_dodag_version_error_count(const di_node_t *node) {
	return node->dodag_version_decrease_errors;
}

int node_get_ip_mismatch_error_count(const di_node_t *node) {
	return node->ip_mismatch_errors;
}

int node_get_dodag_mismatch_error_count(const di_node_t *node) {
	return node->dodag_mismatch_errors;
}