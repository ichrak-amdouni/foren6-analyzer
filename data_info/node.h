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

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
	addr_wpan_t wpan_address;
} di_node_ref_t;

typedef struct di_node_key {
	di_node_ref_t ref;
	uint32_t version;
} di_node_key_t;

typedef struct di_node di_node_t;

size_t node_sizeof();

void node_init(void *data, void *key, size_t key_size);
di_node_t *node_dup(di_node_t *node);

void node_set_key(di_node_t *node, const di_node_key_t *key);
void node_set_local_ip(di_node_t *node, addr_ipv6_t address);
void node_set_global_ip(di_node_t *node, addr_ipv6_t address);
void node_add_route(di_node_t *node, const di_prefix_t *route_prefix, addr_wpan_t via_node);
void node_del_route(di_node_t *node, const di_prefix_t *route_prefix);
void node_set_metric(di_node_t* node, const di_metric_t* metric);
void node_set_rank(di_node_t *node, uint16_t rank);
void node_set_grounded(di_node_t *node, bool grounded);
void node_set_dodag(di_node_t *node, const di_dodag_ref_t *dodag_ref);
void node_set_user_data(di_node_t *node, void *data);

void node_update_ip(di_node_t *node, const di_prefix_t *prefix);

const di_node_key_t *node_get_key(const di_node_t *node);
addr_wpan_t node_get_mac64(const di_node_t *node);
const addr_ipv6_t* node_get_local_ip(const di_node_t *node);
const addr_ipv6_t* node_get_global_ip(const di_node_t *node);
const di_route_list_t* node_get_routes(const di_node_t *node);
const di_metric_t* node_get_metric(const di_node_t* node);
uint16_t node_get_rank(const di_node_t *node);
bool node_get_grounded(const di_node_t *node);
const di_dodag_ref_t * node_get_dodag(const di_node_t *node);
void *node_get_user_data(const di_node_t *node);


#ifdef	__cplusplus
}
#endif

#endif	/* NODE_H */

