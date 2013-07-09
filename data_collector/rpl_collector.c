#include "rpl_collector.h"
#include "rpl_event_callbacks.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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

	//node = node_hash_get(&collected_data->nodes, src_wpan_address, true);
	node = hash_value(collected_data->nodes, hash_key_make(src_wpan_address), HVM_CreateIfNonExistant, &node_created);
	node->wpan_address = src_wpan_address;

	di_dodag_key_t dodag_key = {dio->dodagid, dio->version_number};
	//dodag = dodag_hash_get(&collected_data->dodags, &dio->dodagid, dio->version_number, true);
	dodag = hash_value(collected_data->dodags, hash_key_make(dodag_key), HVM_CreateIfNonExistant, &dodag_created);
	dodag->dodag_key = dodag_key;
	
	//rpl_instance = rpl_instance_get(&collected_data->rpl_instances, dio->rpl_instance_id, true);
	rpl_instance = hash_value(collected_data->rpl_instances, hash_key_make(dio->rpl_instance_id), HVM_CreateIfNonExistant, &rpl_instance_created);
	rpl_instance->instance_id = dio->rpl_instance_id;
	
	dodag->rpl_instance = rpl_instance->instance_id;
	//assert(dodag_hash_add(&rpl_instance->dodags, dodag, false) == dodag);
	hash_add(rpl_instance->dodags, hash_key_make(dodag_key), &dodag_key, NULL, HAM_OverwriteIfExists, NULL);
	
	//Manage new version of DODAG
	if(node->dodag.version >= 0 && node->dodag.version < dio->version_number) {
		//The node had a DODAG with a older version, remove it from the old DODAG and add it to the new one
		//assert(node_hash_remove(&node->dodag->nodes, node->wpan_address) == node);
		hash_iterator_ptr previous_dodag_it = hash_begin(NULL, NULL);
		di_dodag_t *previous_dodag;
		assert(hash_find(collected_data->dodags, hash_key_make(node->dodag), previous_dodag_it));
		previous_dodag = hash_it_value(previous_dodag_it);
		hash_delete(previous_dodag->nodes, hash_key_make(node->wpan_address));
		hash_it_destroy(previous_dodag_it);
		//assert(node_hash_add(&dodag->nodes, node, false) == node);	//the node should not be already in the new dodag
		hash_add(dodag->nodes, hash_key_make(node->wpan_address), &node->wpan_address, NULL, HAM_OverwriteIfExists, NULL);
	} else if(node->dodag.version < 0) {
		//The node was not attached to a dodag, so add to it
		//assert(node_hash_add(&dodag->nodes, node, false) == node);	//the node should not be already in the new dodag
		hash_add(dodag->nodes, hash_key_make(node->wpan_address), &node->wpan_address, NULL, HAM_OverwriteIfExists, NULL);
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

	//child = node_hash_get(&collected_data->nodes, src_wpan_address, true);
	child = hash_value(collected_data->nodes, hash_key_make(src_wpan_address), HVM_CreateIfNonExistant, &child_created);
	child->wpan_address = src_wpan_address;
	child->local_address = *src_ip_address;

	//parent = node_hash_get(&collected_data->nodes, dst_wpan_address, true);
	parent = hash_value(collected_data->nodes, hash_key_make(dst_wpan_address), HVM_CreateIfNonExistant, &parent_created);
	parent->wpan_address = dst_wpan_address;
	parent->local_address = *dst_ip_address;
	
	if(transit && transit->path_lifetime > 0) {
		//Clear parents list
		//link_update(link_hash_get(&collected_data->links, child, parent, true), time(NULL), 1);
		di_link_addr_pair_t link_key;
		link_key.child = child->wpan_address;
		link_key.parent = parent->wpan_address;
		new_link = hash_value(collected_data->links, hash_key_make(link_key), HVM_CreateIfNonExistant, &link_created);
		new_link->key = link_key;

		link_update(new_link, time(NULL), 1);
	} else if(transit && transit->path_lifetime == 0) {
		//No-Path DAO
		if(target && !addr_compare_ip_len(&child->global_address, &target->target, target->target_bit_length)) {
			//link_hash_del(&collected_data->links, child, parent);
			di_link_addr_pair_t link_key;
			link_key.child = child->wpan_address;
			link_key.parent = parent->wpan_address;
			hash_iterator_ptr iterator = hash_begin(NULL, NULL);
			if(hash_find(collected_data->links, hash_key_make(link_key), iterator)) {
				rpl_event_link_deleted(hash_it_value(iterator));
				hash_it_delete_value(iterator);
				hash_it_destroy(iterator);
			}
			//fprintf(stderr, "No-Path DAO, child = 0x%llX, parent = 0x%llX\n", child->wpan_address, parent->wpan_address);
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
			route_el->via_node = child->wpan_address;
		} else {	//No-path DAO
			route_remove(&parent->routes, route);
		}
	}

	if(dao->dodagid_present && child->dodag.version >= 0) {
		//assert((dodag = dodag_hash_get(&collected_data->dodags, &dao->dodagid, child->dodag->dodag_key.version, false)) != NULL);
		assert((dodag = hash_value(collected_data->dodags, hash_key_make(child->dodag), HVM_FailIfNonExistant, &dodag_created)) != NULL);
		//dodag->dodag_key = child->dodag;
	} else dodag = NULL;
	
	//rpl_instance = rpl_instance_get(&collected_data->rpl_instances, dao->rpl_instance_id, true);
	rpl_instance =  hash_value(collected_data->rpl_instances, hash_key_make(dao->rpl_instance_id), HVM_CreateIfNonExistant, &rpl_instance_created);
	rpl_instance->instance_id = dao->rpl_instance_id;
	
	if(dodag) {
		dodag->rpl_instance = rpl_instance->instance_id;
		//assert(dodag_hash_add(&rpl_instance->dodags, dodag, false) == dodag);
		hash_add(rpl_instance->dodags, hash_key_make(dodag->dodag_key), dodag, NULL, HAM_OverwriteIfExists, NULL);
		
		child->dodag = dodag->dodag_key;
		//assert(node_hash_add(&dodag->nodes, child, false) == child);
		hash_add(dodag->nodes, hash_key_make(child->wpan_address), child, NULL, HAM_OverwriteIfExists, NULL);
		
		parent->dodag = dodag->dodag_key;
		//assert(node_hash_add(&dodag->nodes, parent, false) == parent);
		hash_add(dodag->nodes, hash_key_make(parent->wpan_address), parent, NULL, HAM_OverwriteIfExists, NULL);
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

	//node = node_hash_get(&collected_data->nodes, src_wpan_address, true);
	node = hash_value(collected_data->nodes, hash_key_make(src_wpan_address), HVM_CreateIfNonExistant, &node_created);
	node->wpan_address = src_wpan_address;
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

	//src = node_hash_get(&collected_data->nodes, src_wpan_address, true);
	src = hash_value(collected_data->nodes, hash_key_make(src_wpan_address), HVM_CreateIfNonExistant, &src_created);
	src->wpan_address = src_wpan_address;
	if(!rpl_info || rpl_info->sender_rank == 0) {
		//src->global_address = *src_ip_address;
	} else if(rpl_info) {
		//src->rank = rpl_info->sender_rank;
	}
	
	if(dst_wpan_address != 0 && dst_wpan_address != ADDR_MAC64_BROADCAST) {
		//dst = node_hash_get(&collected_data->nodes, dst_wpan_address, true);
		dst = hash_value(collected_data->nodes, hash_key_make(dst_wpan_address), HVM_CreateIfNonExistant, &dst_created);
		dst->wpan_address = dst_wpan_address;
		/* Add the parent node to the parents list of the child node if not already done */

		//Bug: parent <-> child not following DAO messages
		if(rpl_info) {
			if(rpl_info->packet_toward_root) {
				//link_update(link_hash_get(&collected_data->links, src, dst, true), time(NULL), 1);
				di_link_addr_pair_t link_key;
				link_key.child = src->wpan_address;
				link_key.parent = dst->wpan_address;
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
