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

void rpl_collector_parse_dio(packet_info_t pkt_info,
		rpl_dio_t* dio,
		rpl_dio_opt_config_t* dodag_config,
		rpl_dio_opt_metric_t* metric,
		rpl_dio_opt_prefix_t* prefix,
		rpl_dio_opt_route_t* route_info)
{
	//fprintf(stderr, "Received DIO\n");

	di_node_t *node;
	di_dodag_t *dodag;
	di_rpl_instance_t *rpl_instance;

	bool node_created;
	bool dodag_created;
	bool rpl_instance_created;

	//Get node, dodag and rpl_instance using their keys. If the requested object does not exist, created it.
	di_node_ref_t node_ref;
	node_ref_init(&node_ref, pkt_info.src_wpan_address);
	node = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &node_created);
	node_add_packet_count(node, 1);
	node_set_dtsn(node, dio->dtsn);

	di_dodag_ref_t dodag_ref;
	dodag_ref_init(&dodag_ref, dio->dodagid, dio->version_number);
	dodag = rpldata_get_dodag(&dodag_ref, HVM_CreateIfNonExistant, &dodag_created);

	di_rpl_instance_ref_t rpl_instance_ref;
	rpl_instance_ref_init(&rpl_instance_ref, dio->rpl_instance_id);
	rpl_instance = rpldata_get_rpl_instance(&rpl_instance_ref, HVM_CreateIfNonExistant, &rpl_instance_created);

	rpl_instance_add_dodag(rpl_instance, dodag);

	//Manage new version of DODAG
	if(node_get_dodag(node) && node_get_dodag(node)->version < dio->version_number) {
		//The node had a DODAG with a older version, remove it from the old DODAG and add it to the new one
		di_dodag_t *previous_dodag;
		di_dodag_ref_t previous_dodag_ref;

		dodag_ref_init(&previous_dodag_ref, node_get_dodag(node)->dodagid, node_get_dodag(node)->version);
		previous_dodag = rpldata_get_dodag(&previous_dodag_ref, HVM_FailIfNonExistant, NULL);
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
}

void rpl_collector_parse_dao(packet_info_t pkt_info,
		rpl_dao_t* dao,
		rpl_dao_opt_target_t* target,
		rpl_dao_opt_transit_t *transit)
{

	di_node_t *child, *parent;
	di_dodag_t *dodag;
	di_rpl_instance_t *rpl_instance;
	di_link_t *new_link = NULL;
	di_link_t *old_link = NULL;

	bool child_created = false;
	bool parent_created = false;
	bool dodag_created = false;
	bool rpl_instance_created = false;
	bool link_created = false;

	//fprintf(stderr, "Received DAO\n");

	di_node_ref_t node_ref;
	node_ref_init(&node_ref, pkt_info.src_wpan_address);
	child = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &child_created);
	node_add_packet_count(child, 1);

	node_ref_init(&node_ref, pkt_info.dst_wpan_address);
	parent = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &parent_created);

	di_rpl_instance_ref_t rpl_instance_ref;
	rpl_instance_ref_init(&rpl_instance_ref, dao->rpl_instance_id);
	rpl_instance = rpldata_get_rpl_instance(&rpl_instance_ref, HVM_CreateIfNonExistant, &rpl_instance_created);


	if(dao->dodagid_present && node_get_dodag(child)) {
		di_dodag_ref_t dodag_ref;
		dodag_ref_init(&dodag_ref, node_get_dodag(child)->dodagid, node_get_dodag(child)->version);
		dodag = rpldata_get_dodag(&dodag_ref, HVM_FailIfNonExistant, &dodag_created);
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

		di_link_ref_t link_ref;
		link_ref_init(&link_ref, (di_node_ref_t){node_get_mac64(child)}, (di_node_ref_t){node_get_mac64(parent)});
		new_link = rpldata_get_link(&link_ref, HVM_CreateIfNonExistant, &link_created);
		link_update(new_link, time(NULL), 1);
		//link_set_metric(new_link, node_get_metric(child));
	} else if(transit && transit->path_lifetime == 0) {
		//No-Path DAO
		if(target && !addr_compare_ip_len(node_get_global_ip(child), &target->target, target->target_bit_length)) {
			di_link_ref_t link_ref;
			link_ref_init(&link_ref, (di_node_ref_t){node_get_mac64(child)}, (di_node_ref_t){node_get_mac64(parent)});
			old_link = rpldata_del_link(&link_ref);
			if(old_link) {
				rpl_event_link(old_link, RET_Deleted);
				free(old_link);
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
			node_del_route(parent, &route, node_get_mac64(child));
		}

		if(addr_compare_ip(&target->target, node_get_global_ip(child)) == 0) {
			node_set_dao_seq(child, dao->dao_sequence);
			node_update_dao_interval(child, pkt_info.timestamp);
		}
	}
}

void rpl_collector_parse_dis(packet_info_t pkt_info,
		rpl_dis_opt_info_req_t *request)
{
	bool node_created;
	di_node_t *node;

	//fprintf(stderr, "Received DIS\n");

	di_node_ref_t node_ref;
	node_ref_init(&node_ref, pkt_info.src_wpan_address);
	node = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &node_created);  //nothing to do with the node, but be sure it exists in the node list
	node_add_packet_count(node, 1);
}

void rpl_collector_parse_data(packet_info_t pkt_info,
		rpl_hop_by_hop_opt_t* rpl_info)
{
	di_node_t *src, *dst = NULL;
	di_link_t *link;

	bool src_created, dst_created;
	bool link_created;

	//fprintf(stderr, "Received Data\n");

	di_node_ref_t node_ref;
	node_ref_init(&node_ref, pkt_info.src_wpan_address);
	src = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &src_created);
	node_add_packet_count(src, 1);

	if(!rpl_info || rpl_info->sender_rank == 0) {
		//src->global_address = *src_ip_address;
	} else if(rpl_info) {
		//src->rank = rpl_info->sender_rank;
	}

	if(pkt_info.dst_wpan_address != 0 && pkt_info.dst_wpan_address != ADDR_MAC64_BROADCAST) {
		node_ref_init(&node_ref, pkt_info.dst_wpan_address);
		dst = rpldata_get_node(&node_ref, HVM_CreateIfNonExistant, &dst_created);

		/* Add the parent node to the parents list of the child node if not already done */

		if(rpl_info) {
			if(rpl_info->packet_toward_root) {
				di_link_ref_t link_ref;
				link_ref_init(&link_ref, (di_node_ref_t){node_get_mac64(src)}, (di_node_ref_t){node_get_mac64(dst)});
				link = rpldata_get_link(&link_ref, HVM_CreateIfNonExistant, &link_created);
				link_update(link, time(NULL), 1);
				//link_set_metric(new_link, node_get_metric(src));

				di_prefix_t route;
				route.expiration_time = 0;
				route.length = 128;
				route.prefix = pkt_info.src_ip_address;
				node_add_route(dst, &route, node_get_mac64(src));
			}
		}
	}
}
