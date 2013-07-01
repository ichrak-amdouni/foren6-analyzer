#include "rpl_collector.h"
#include "../data_info/address.h"
#include "rpl_event_callbacks.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../utlist.h"

static rpl_collector_t collected_data;

static void rpl_collector_dump_node(di_node_t *rpl_node, int depth);

void rpl_collector_init() {
	memset(&collected_data, 0, sizeof(collected_data));
}

static void rpl_collector_dump_node(di_node_t *rpl_node, int depth) {
	di_link_t *subnode_address;
	char ip_buffer[INET6_ADDRSTRLEN];
	int i;
	
	//Loop protection
	if(depth > 1 || rpl_node == NULL)
		return;
	
	inet_ntop(AF_INET6, &rpl_node->local_address, ip_buffer, INET6_ADDRSTRLEN);
	for(i = 0; i < depth; i++)
		fputc(' ', stderr);

	inet_ntop(AF_INET6, &rpl_node->local_address, ip_buffer, INET6_ADDRSTRLEN);
	fprintf(stderr, "   Node depth: %d, wpan address: 0x%0*llX, link-local ip: %s, global ip: ", depth, 16, rpl_node->wpan_address, ip_buffer);
	inet_ntop(AF_INET6, &rpl_node->global_address, ip_buffer, INET6_ADDRSTRLEN);
	fprintf(stderr, "%s rank: %d, etx: %f, grounded: %d\n", ip_buffer, rpl_node->rank, rpl_node->metric.value / 128.0, rpl_node->grounded);

/*
	LL_FOREACH(rpl_node->children, subnode_address) {
		rpl_collector_dump_node(node_hash_get(&collected_data.nodes, subnode_address->other_address, false), depth+1);
	}
*/
}

void rpl_collector_dump() {
	di_rpl_instance_el_t *rpl_instance_el, *rpl_instance_tmp;
	di_dodag_el_t *rpl_dodag_el, *rpl_dodag_tmp;
	di_node_el_t *rpl_node_el, *rpl_node_tmp;
	di_rpl_instance_t *rpl_instance;
	di_dodag_t *rpl_dodag;
	di_node_t *rpl_node;
	di_route_el_t *rpl_route;
	char ip_buffer[INET6_ADDRSTRLEN];
	
	fprintf(stderr, "Sensors infos: -----------------\n");

	HASH_ITER(hh, collected_data.rpl_instances, rpl_instance_el, rpl_instance_tmp) {
		rpl_instance = rpl_instance_el->rpl_instance;
		fprintf(stderr, "RPL Instance: %d, Mode of operation: %d\n", rpl_instance->instance_id, rpl_instance->mode_of_operation);
		HASH_ITER(hh, rpl_instance->dodags, rpl_dodag_el, rpl_dodag_tmp) {
			rpl_dodag = rpl_dodag_el->dodag;
			inet_ntop(AF_INET6, &rpl_dodag->dodag_key.dodagid, ip_buffer, INET6_ADDRSTRLEN);
			fprintf(stderr, "  DODAG: %s, version: %d, min_hop_rank_inc: %d\n", ip_buffer, rpl_dodag->dodag_key.version, rpl_dodag->config.min_hop_rank_inc);


			HASH_ITER(hh, rpl_dodag->nodes, rpl_node_el, rpl_node_tmp) {
				rpl_collector_dump_node(rpl_node_el->node, 0);
			}

		}
	}
	HASH_ITER(hh, collected_data.nodes, rpl_node_el, rpl_node_tmp) {
		rpl_node = rpl_node_el->node;
		inet_ntop(AF_INET6, &rpl_node->local_address, ip_buffer, INET6_ADDRSTRLEN);
		fprintf(stderr, "Node wpan address: 0x%0*llX, link-local ip: %s, global ip: ", 16, rpl_node->wpan_address, ip_buffer);
		inet_ntop(AF_INET6, &rpl_node->global_address, ip_buffer, INET6_ADDRSTRLEN);
		fprintf(stderr, "%s rank: %d, etx: %f, grounded: %d\n", ip_buffer, rpl_node->rank, rpl_node->metric.value / 128.0, rpl_node->grounded);
		
		LL_FOREACH(rpl_node->routes, rpl_route) {
			inet_ntop(AF_INET6, &rpl_route->route_prefix.prefix, ip_buffer, INET6_ADDRSTRLEN);
			fprintf(stderr, "  Route %s/%d via wpan 0x%0*llX\n", ip_buffer, rpl_route->route_prefix.length, 16, rpl_route->via_node);
		}
	}
	
	FILE *dotout = fopen("graph.dot", "w");
	if(dotout) {
		di_link_el_t *link, *link_tmp;
		fprintf(dotout, "digraph \"dodag\"{\n");
		HASH_ITER(hh, collected_data.links, link, link_tmp) {
			fprintf(dotout, "\"%llX\" -> \"%llX\"\n", link->link->key.child->wpan_address, link->link->key.parent->wpan_address);
		}
		fprintf(dotout, "}\n");
		fclose(dotout);
	}
	fprintf(stderr, "End Sensors infos --------------\n\n");
}

rpl_collector_t *rpl_collector_get() {
	return &collected_data;
}

void rpl_collector_parse_dio(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
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

	node = node_hash_get(&collected_data.nodes, src_wpan_address, true);

	
	dodag = dodag_hash_get(&collected_data.dodags, &dio->dodagid, dio->version_number, true);
	rpl_instance = rpl_instance_get(&collected_data.rpl_instances, dio->rpl_instance_id, true);
	
	dodag->rpl_instance = rpl_instance;
	assert(dodag_hash_add(&rpl_instance->dodags, dodag, false) == dodag);
	
	//Manage new version of DODAG
	if(node->dodag != NULL && node->dodag->dodag_key.version < dio->version_number) {
		//The node had a DODAG with a older version, remove it from the old DODAG and add it to the new one
		assert(node_hash_remove(&node->dodag->nodes, node->wpan_address) == node);
		assert(node_hash_add(&dodag->nodes, node, false) == node);	//the node should not be already in the new dodag
	} else if(node->dodag == NULL) {
		//The node was not attached to a dodag, so add to it
		assert(node_hash_add(&dodag->nodes, node, false) == node);	//the node should not be already in the new dodag
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
	di_node_t *child, *parent;
	di_dodag_t *dodag;
	di_rpl_instance_t *rpl_instance;
	
	//fprintf(stderr, "Received DAO\n");

	child = node_hash_get(&collected_data.nodes, src_wpan_address, true);
	child->local_address = *src_ip_address;

	parent = node_hash_get(&collected_data.nodes, dst_wpan_address, true);
	parent->local_address = *dst_ip_address;
	
	if(transit && transit->path_lifetime > 0) {
		//Clear parents list
		link_update(link_hash_get(&collected_data.links, child, parent, true), time(NULL), 1);
	} else if(transit && transit->path_lifetime == 0) {
		//No-Path DAO
		if(target && !addr_compare_ip_len(&child->global_address, &target->target, target->target_bit_length)) {
			link_hash_del(&collected_data.links, child, parent);
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

	if(dao->dodagid_present && child->dodag != 0)
		assert((dodag = dodag_hash_get(&collected_data.dodags, &dao->dodagid, child->dodag->dodag_key.version, false)) != NULL);
	else dodag = NULL;
	
	rpl_instance = rpl_instance_get(&collected_data.rpl_instances, dao->rpl_instance_id, true);
	
	if(dodag) {
		dodag->rpl_instance = rpl_instance;
		assert(dodag_hash_add(&rpl_instance->dodags, dodag, false) == dodag);
		
		child->dodag = dodag;
		assert(node_hash_add(&dodag->nodes, child, false) == child);
		
		parent->dodag = dodag;
		assert(node_hash_add(&dodag->nodes, parent, false) == parent);
	}
	
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
	di_node_t *node;

	//fprintf(stderr, "Received DIS\n");

	node = node_hash_get(&collected_data.nodes, src_wpan_address, true);
	node->local_address = *src_ip_address;
	rpl_event_node_updated(node);
}

void rpl_collector_parse_data(uint64_t src_wpan_address, uint64_t dst_wpan_address,
		struct in6_addr *src_ip_address, struct in6_addr *dst_ip_address,
		rpl_hop_by_hop_opt_t* rpl_info, int packet_id)
{
	di_node_t *src, *dst;
	
	//fprintf(stderr, "Received Data\n");

	src = node_hash_get(&collected_data.nodes, src_wpan_address, true);
	if(!rpl_info || rpl_info->sender_rank == 0) {
		//src->global_address = *src_ip_address;
	} else if(rpl_info) {
		//src->rank = rpl_info->sender_rank;
	}
	
	if(dst_wpan_address != 0 && dst_wpan_address != ADDR_MAC64_BROADCAST) {
		dst = node_hash_get(&collected_data.nodes, dst_wpan_address, true);
		/* Add the parent node to the parents list of the child node if not already done */

		//Bug: parent <-> child not following DAO messages
		if(rpl_info) {
			if(rpl_info->packet_toward_root) {
				link_update(link_hash_get(&collected_data.links, src, dst, true), time(NULL), 1);

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
	
	rpl_event_node_updated(src);
	rpl_event_node_updated(dst);
}
