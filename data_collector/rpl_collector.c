#include "rpl_collector.h"
#include "rpl_event_callbacks.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../data_info/address.h"
#include "../data_info/node.h"
#include "../data_info/dodag.h"
#include "../data_info/rpl_instance.h"
#include "../data_info/route.h"
#include "../data_info/link.h"
#include "../data_info/metric.h"
#include "../data_info/rpl_data.h"

void rpl_collector_parse_dio(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
		rpl_dio_t* dio,
		rpl_dio_opt_config_t* dodag_config,
		rpl_dio_opt_metric_t* metric,
		rpl_dio_opt_prefix_t* prefix,
		rpl_dio_opt_route_t* route_info)
{
	//fprintf(stderr, "Received DIO\n");
	di_rpl_data_t *collected_data = rpldata_get();

	di_node_t *node;
	di_dodag_t *dodag;
	di_rpl_instance_t *rpl_instance;

	bool node_created;
	bool dodag_created;
	bool rpl_instance_created;

	di_node_key_t node_key = {{src_wpan_address}, 0};
	node = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &node_created);
	node->key = node_key;

	di_dodag_key_t dodag_key = {{dio->dodagid, dio->version_number}, 0};
	dodag = hash_value(collected_data->dodags, hash_key_make(dodag_key), HVM_CreateIfNonExistant, &dodag_created);
	dodag->key = dodag_key;

	di_rpl_instance_key_t rpl_instance_key = {{dio->rpl_instance_id}, 0};
	rpl_instance = hash_value(collected_data->rpl_instances, hash_key_make(rpl_instance_key), HVM_CreateIfNonExistant, &rpl_instance_created);
	rpl_instance->key = rpl_instance_key;

	hash_add(rpl_instance->dodags, hash_key_make(dodag_key.ref), &dodag_key.ref, NULL, HAM_OverwriteIfExists, NULL);
	dodag->rpl_instance = rpl_instance->key.ref;

	//Manage new version of DODAG
	if(node->dodag.version >= 0 && node->dodag.version < dio->version_number) {
		//The node had a DODAG with a older version, remove it from the old DODAG and add it to the new one
		hash_iterator_ptr previous_dodag_it = hash_begin(NULL, NULL);
		di_dodag_t *previous_dodag;
		di_dodag_key_t dodag_key = {node->dodag, 0};
		assert(hash_find(collected_data->dodags, hash_key_make(dodag_key), previous_dodag_it));
		previous_dodag = hash_it_value(previous_dodag_it);
		hash_delete(previous_dodag->nodes, hash_key_make(node->key.ref));
		hash_it_destroy(previous_dodag_it);

		hash_add(dodag->nodes, hash_key_make(node->key.ref), &node->key.ref, NULL, HAM_OverwriteIfExists, NULL);
		node->dodag = dodag->key.ref;
	} else if(node->dodag.version < 0) {
		//The node was not attached to a dodag, so add to it
		hash_add(dodag->nodes, hash_key_make(node->key.ref), &node->key.ref, NULL, HAM_OverwriteIfExists, NULL);
		node->dodag = dodag->key.ref;
	}

	node->rank = dio->rank;
	node->grounded = dio->grounded;
	rpl_instance->mode_of_operation = dio->mode_of_operation;

	if(prefix) {
		dodag->prefix.length = prefix->prefix_bit_length;
		dodag->prefix.prefix = prefix->prefix;
		//dodag->prefix.expiration_time = time(NULL) + prefix->preferred_lifetime;
		node->global_address = addr_get_global_ip_from_mac64(dodag->prefix, src_wpan_address);
	}
	if(metric && metric->type == RDOMT_ETX) {
		node->metric.type = metric_get_type("ETX");
		node->metric.value = metric->value;
	}
	if(dodag_config) {
		dodag->config.auth_enabled = dodag_config->auth_enabled;
		dodag->config.default_lifetime = dodag_config->default_lifetime;
		dodag->config.dio_interval_max = dodag_config->dio_interval_max;
		dodag->config.dio_interval_min = dodag_config->dio_interval_min;
		dodag->config.dio_redundancy_constant = dodag_config->dio_redundancy_constant;
		dodag->config.lifetime_unit = dodag_config->lifetime_unit;
		dodag->config.max_rank_inc = dodag_config->max_rank_inc;
		dodag->config.min_hop_rank_inc = dodag_config->min_hop_rank_inc;
		dodag->config.objective_function = dodag_config->objective_function;
		dodag->config.path_control_size = dodag_config->path_control_size;
	}

	if(node_created)
		rpl_event_node_created(node);
	if(dodag_created)
		rpl_event_dodag_created(dodag);
	if(rpl_instance_created)
		rpl_event_rpl_instance_created(rpl_instance);

	rpl_event_node_updated(node);
	rpl_event_dodag_updated(dodag);
	rpl_event_rpl_instance_updated(rpl_instance);
}

void rpl_collector_parse_dao(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
		rpl_dao_t* dao,
		rpl_dao_opt_target_t* target,
		rpl_dao_opt_transit_t *transit)
{
	di_rpl_data_t *collected_data = rpldata_get();

	di_node_t *child, *parent;
	di_dodag_t *dodag;
	di_rpl_instance_t *rpl_instance;
	di_link_t *new_link = NULL;

	bool child_created = false;
	bool parent_created = false;
	bool dodag_created = false;
	bool rpl_instance_created = false;
	bool link_created = false;

	//fprintf(stderr, "Received DAO\n");

	di_node_key_t node_key = {{src_wpan_address}, 0};
	child = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &child_created);
	child->key = node_key;

	node_key = (di_node_key_t){{dst_wpan_address}, 0};
	parent = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &parent_created);
	parent->key = node_key;

	di_rpl_instance_key_t rpl_instance_key = {{dao->rpl_instance_id}, 0};
	rpl_instance = hash_value(collected_data->rpl_instances, hash_key_make(rpl_instance_key), HVM_CreateIfNonExistant, &rpl_instance_created);
	rpl_instance->key = rpl_instance_key;

	if(dao->dodagid_present && child->dodag.version >= 0) {
		di_dodag_key_t dodag_key = {child->dodag, 0};
		assert((dodag = hash_value(collected_data->dodags, hash_key_make(dodag_key), HVM_FailIfNonExistant, &dodag_created)) != NULL);
		//dodag->dodag_key = child->dodag;
	} else dodag = NULL;

	if(dodag) {
		dodag->rpl_instance = rpl_instance->key.ref;
		hash_add(rpl_instance->dodags, hash_key_make(dodag->key.ref), &dodag->key.ref, NULL, HAM_OverwriteIfExists, NULL);

		assert(!memcmp(&child->dodag, &dodag->key.ref, sizeof(child->dodag)));
		assert(child->global_address.__in6_u.__u6_addr32[0] != 0);
		//child->global_address = addr_get_global_ip_from_mac64(dodag->prefix, child->key.ref.wpan_address);
		hash_add(dodag->nodes, hash_key_make(child->key.ref), &child->key.ref, NULL, HAM_OverwriteIfExists, NULL);

		parent->dodag = dodag->key.ref;
		hash_add(dodag->nodes, hash_key_make(parent->key.ref), &parent->key.ref, NULL, HAM_OverwriteIfExists, NULL);
	}

	if(transit && transit->path_lifetime > 0) {
		//Clear parents list

		di_link_key_t link_key = {{{child->key.ref.wpan_address}, {parent->key.ref.wpan_address}}, 0};
		new_link = hash_value(collected_data->links, hash_key_make(link_key), HVM_CreateIfNonExistant, &link_created);
		new_link->key = link_key;

		link_update(new_link, time(NULL), 1);
	} else if(transit && transit->path_lifetime == 0) {
		//No-Path DAO
		if(target && !addr_compare_ip_len(&child->global_address, &target->target, target->target_bit_length)) {
			//link_hash_del(&collected_data->links, child, parent);
			di_link_key_t link_key = {{{child->key.ref.wpan_address}, {parent->key.ref.wpan_address}}, 0};
			hash_iterator_ptr iterator = hash_begin(NULL, NULL);
			if(hash_find(collected_data->links, hash_key_make(link_key), iterator)) {
				rpl_event_link_deleted(hash_it_value(iterator));
				hash_it_delete_value(iterator);
				hash_it_destroy(iterator);
			}
			//fprintf(stderr, "No-Path DAO, child = 0x%llX, parent = 0x%llX\n", child->key.ref.wpan_address, parent->key.ref.wpan_address);
		} else if((child->key.ref.wpan_address & 0xFF) == 0x0D) {
			fprintf(stderr, "No-Path DAO, child = 0x%llX, parent = 0x%llX, target = %d\n", child->key.ref.wpan_address, parent->key.ref.wpan_address, target->target.__in6_u.__u6_addr8[15]);
		}
	}

	if(target && transit) {
		di_prefix_t route;
		di_route_el_t *route_el;
		route.expiration_time = transit->path_lifetime;
		route.length = target->target_bit_length;
		route.prefix = target->target;

		if(transit->path_lifetime > 0) {
			route_el = route_add(&parent->routes, route, true);
			route_el->via_node = child->key.ref.wpan_address;
		} else {	//No-path DAO
			route_remove(&parent->routes, route);
		}
	}

	if(child_created)
		rpl_event_node_created(child);
	if(parent_created)
		rpl_event_node_created(parent);

	if(dodag && dodag_created)
		rpl_event_dodag_created(dodag);

	if(rpl_instance_created)
		rpl_event_rpl_instance_created(rpl_instance);

	if(new_link && link_created)
		rpl_event_link_created(new_link);

	rpl_event_node_updated(child);
	rpl_event_node_updated(parent);
	if(dodag)
		rpl_event_dodag_updated(dodag);
	rpl_event_rpl_instance_updated(rpl_instance);
}

void rpl_collector_parse_dis(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
		rpl_dis_opt_info_req_t *request)
{
	di_rpl_data_t *collected_data = rpldata_get();

	di_node_t *node;
	bool node_created;

	//fprintf(stderr, "Received DIS\n");

	di_node_key_t node_key = {{src_wpan_address}, 0};
	node = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &node_created);
	node->key = node_key;
	node->local_address = *src_ip_address;

	if(node_created)
		rpl_event_node_created(node);

	rpl_event_node_updated(node);
}

void rpl_collector_parse_data(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
		rpl_hop_by_hop_opt_t* rpl_info, int packet_id)
{
	di_rpl_data_t *collected_data = rpldata_get();

	di_node_t *src, *dst;
	di_link_t *new_link = NULL;

	bool src_created, dst_created;
	bool link_created;

	//fprintf(stderr, "Received Data\n");

	di_node_key_t node_key = {{src_wpan_address}, 0};
	src = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &src_created);
	src->key = node_key;

	if(!rpl_info || rpl_info->sender_rank == 0) {
		//src->global_address = *src_ip_address;
	} else if(rpl_info) {
		//src->rank = rpl_info->sender_rank;
	}

	if(dst_wpan_address != 0 && dst_wpan_address != ADDR_MAC64_BROADCAST) {
		node_key = (di_node_key_t){{dst_wpan_address}, 0};
		dst = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &dst_created);
		dst->key = node_key;
		//dst->global_address = *dst_ip_address;

		/* Add the parent node to the parents list of the child node if not already done */

		//Bug: parent <-> child not following DAO messages
		if(rpl_info) {
			if(rpl_info->packet_toward_root) {
				di_link_key_t link_key = {{{src->key.ref.wpan_address}, {dst->key.ref.wpan_address}}, 0};
				new_link = hash_value(collected_data->links, hash_key_make(link_key), HVM_CreateIfNonExistant, &link_created);
				new_link->key = link_key;

				di_prefix_t route;
				di_route_el_t *route_el;
				route.expiration_time = 0;
				route.length = 128;
				route.prefix = *src_ip_address;
				route_el = route_add(&dst->routes, route, true);
				route_el->via_node = src_wpan_address;
			}
		}
	}

	if(src_created)
		rpl_event_node_created(src);
	if(dst_created)
		rpl_event_node_created(dst);
	if(new_link && link_created)
		rpl_event_link_created(new_link);

	rpl_event_node_updated(src);
	rpl_event_node_updated(dst);
}
