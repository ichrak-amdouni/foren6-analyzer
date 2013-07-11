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

	//Get node, dodag and rpl_instance using their keys. If the requested object does not exist, created it.
	di_node_key_t node_key = {{src_wpan_address}, 0};
	node = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &node_created);

	di_dodag_key_t dodag_key = {{dio->dodagid, dio->version_number}, 0};
	dodag = hash_value(collected_data->dodags, hash_key_make(dodag_key), HVM_CreateIfNonExistant, &dodag_created);

	di_rpl_instance_key_t rpl_instance_key = {{dio->rpl_instance_id}, 0};
	rpl_instance = hash_value(collected_data->rpl_instances, hash_key_make(rpl_instance_key), HVM_CreateIfNonExistant, &rpl_instance_created);

	rpl_instance_add_dodag(rpl_instance, dodag);

	//Manage new version of DODAG
	if(node_get_dodag(node) && node_get_dodag(node)->version < dio->version_number) {
		//The node had a DODAG with a older version, remove it from the old DODAG and add it to the new one
		di_dodag_t *previous_dodag;
		di_dodag_key_t previous_dodag_key = {*node_get_dodag(node), 0};

		previous_dodag = hash_value(collected_data->dodags, hash_key_make(previous_dodag_key), HVM_FailIfNonExistant, NULL);
		assert(previous_dodag != NULL);
		dodag_del_node(previous_dodag, node);

		dodag_add_node(dodag, node);
	} else if(node_get_dodag(node) == NULL) {
		//The node was not attached to a dodag, so add to it
		dodag_add_node(dodag, node);
	}

	node_set_rank(node, dio->rank);
	node_set_grounded(node, dio->grounded);
	rpl_instance_set_mop(rpl_instance, dio->mode_of_operation);

	if(prefix) {
		di_prefix_t dodag_prefix;
		dodag_prefix.length = prefix->prefix_bit_length;
		dodag_prefix.prefix = prefix->prefix;
		dodag_prefix.expiration_time = 0;
		//dodag->prefix.expiration_time = time(NULL) + prefix->preferred_lifetime;
		dodag_set_prefix(dodag, &dodag_prefix);
	}
	if(metric && metric->type == RDOMT_ETX) {
		di_metric_t metric_value = {metric_get_type("ETX"), metric->value};
		node_set_metric(node, &metric_value);
	} else if(metric) {
		fprintf(stderr, "Warning: metric is not ETX (%d)!\n", metric->type);
	}

	if(dodag_config) {
		di_dodag_config_t config;
		config.auth_enabled = dodag_config->auth_enabled;
		config.default_lifetime = dodag_config->default_lifetime;
		config.dio_interval_max = dodag_config->dio_interval_max;
		config.dio_interval_min = dodag_config->dio_interval_min;
		config.dio_redundancy_constant = dodag_config->dio_redundancy_constant;
		config.lifetime_unit = dodag_config->lifetime_unit;
		config.max_rank_inc = dodag_config->max_rank_inc;
		config.min_hop_rank_inc = dodag_config->min_hop_rank_inc;
		config.objective_function = dodag_config->objective_function;
		config.path_control_size = dodag_config->path_control_size;
		dodag_set_config(dodag, &config);
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

	node_key = (di_node_key_t){{dst_wpan_address}, 0};
	parent = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &parent_created);

	di_rpl_instance_key_t rpl_instance_key = {{dao->rpl_instance_id}, 0};
	rpl_instance = hash_value(collected_data->rpl_instances, hash_key_make(rpl_instance_key), HVM_CreateIfNonExistant, &rpl_instance_created);


	if(dao->dodagid_present && node_get_dodag(child)) {
		di_dodag_key_t dodag_key = {*node_get_dodag(child), 0};
		dodag = hash_value(collected_data->dodags, hash_key_make(dodag_key), HVM_FailIfNonExistant, &dodag_created);
		assert(dodag != NULL);

		rpl_instance_add_dodag(rpl_instance, dodag);

		assert(!memcmp(node_get_dodag(child), &dodag_get_key(dodag)->ref, sizeof(di_dodag_ref_t)));
		assert(!addr_is_ip_any(*node_get_global_ip(child)));

		dodag_add_node(dodag, child);
		dodag_add_node(dodag, parent);
	} else {
		dodag = NULL;
	}

	if(transit && transit->path_lifetime > 0) {
		//Clear parents list

		di_link_key_t link_key = {{{node_get_mac64(child)}, {node_get_mac64(parent)}}, 0};
		new_link = hash_value(collected_data->links, hash_key_make(link_key), HVM_CreateIfNonExistant, &link_created);
		link_update(new_link, time(NULL), 1);
	} else if(transit && transit->path_lifetime == 0) {
		//No-Path DAO
		if(target && !addr_compare_ip_len(node_get_global_ip(child), &target->target, target->target_bit_length)) {
			//link_hash_del(&collected_data->links, child, parent);
			di_link_key_t link_key = {{{node_get_mac64(child)}, {node_get_mac64(parent)}}, 0};
			hash_iterator_ptr iterator = hash_begin(NULL, NULL);
			if(hash_find(collected_data->links, hash_key_make(link_key), iterator)) {
				rpl_event_link_deleted(hash_it_value(iterator));
				hash_it_delete_value(iterator);
				hash_it_destroy(iterator);
			}
			//fprintf(stderr, "No-Path DAO, child = 0x%llX, parent = 0x%llX\n", child->key.ref.wpan_address, parent->key.ref.wpan_address);
		}
	}

	if(target && transit) {
		di_prefix_t route;
		route.expiration_time = transit->path_lifetime;
		route.length = target->target_bit_length;
		route.prefix = target->target;

		if(transit->path_lifetime > 0) {
			node_add_route(parent, &route, node_get_mac64(child));
		} else {	//No-path DAO
			node_del_route(parent, &route);
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

	if(!rpl_info || rpl_info->sender_rank == 0) {
		//src->global_address = *src_ip_address;
	} else if(rpl_info) {
		//src->rank = rpl_info->sender_rank;
	}

	if(dst_wpan_address != 0 && dst_wpan_address != ADDR_MAC64_BROADCAST) {
		node_key = (di_node_key_t){{dst_wpan_address}, 0};
		dst = hash_value(collected_data->nodes, hash_key_make(node_key), HVM_CreateIfNonExistant, &dst_created);

		/* Add the parent node to the parents list of the child node if not already done */

		if(rpl_info) {
			if(rpl_info->packet_toward_root) {
				di_link_key_t link_key = {{{node_get_mac64(src)}, {node_get_mac64(dst)}}, 0};
				new_link = hash_value(collected_data->links, hash_key_make(link_key), HVM_CreateIfNonExistant, &link_created);

				di_prefix_t route;
				route.expiration_time = 0;
				route.length = 128;
				route.prefix = *src_ip_address;
				node_add_route(dst, &route, node_get_mac64(src));
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
