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
#include "link.h"
#include "route.h"
#include "../uthash.h"
#include "dodag.h"

typedef struct di_dodag di_dodag_t;
typedef struct di_dodag_el *di_dodag_hash_t;

typedef struct di_node {
	addr_wpan_t wpan_address;
	uint32_t version;
	
	addr_ipv6_t local_address;
	addr_ipv6_t global_address;
	
	di_route_list_t routes;
	
	uint16_t rank;				//Via DIO
	di_metric_t metric;		//Usually ETX, via DIO with metric
	bool grounded;				//If true, can propagate packet to the root node.
	
	di_dodag_key_t dodag;
	
	void *user_data;
} di_node_t;

typedef struct di_node_el {
	di_node_t *node;
    UT_hash_handle hh;
} di_node_el_t, *di_node_hash_t;

typedef struct di_node_ref {
	addr_wpan_t node;
	struct di_node_ref *next;
} di_node_ref_t, *di_node_ref_list_t;

/**
 * Get the node with specified address in the hashtable;
 * If it does not exist, create it if get_or_create is true, else return NULL.
 * 
 * @param hash hashtable from where to get the node
 * @param wpan_address the address of the node to search
 * @param get_or_create if true, if the node is not found in the hashtable, it will be created. If false, NULL is returned when the node is not found
 * @return the node structure containing data for the specified id and version
 */
di_node_t *node_hash_get(di_node_hash_t *hash, addr_wpan_t wpan_address, bool get_or_create);

/**
 * Add an existing node structure to the hashtable.
 * @param hash hashtable
 * @param node pointer to the bide structure to add
 * @param overwrite_existing if the node was already in the hashtable but with a different pointer, overwrite it only if this parameter is true
 * @return the node in the hashtable. May not be the node parameter if overwrite_existing is false and if there was already existing node with same address.
 */
di_node_t *node_hash_add(di_node_hash_t *hash, di_node_t *node, bool overwrite_existing);

/**
 * Remove a node structure from the hashtable.
 * The node structure is not freed.
 * 
 * @param hash hashtable
 * @param wpan_address the address of the node to remove
 * @return a pointer to the removed node structure or NULL if it was not found
 */
di_node_t *node_hash_remove(di_node_hash_t *hash, addr_wpan_t wpan_address);

/**
 * Remove a node from the hashtable and free its memory
 * If the node exist in the hashtable, this function is equivalent to free(node_hash_remove(...))
 * @param hash hashtable
 * @param wpan_address the address of the node to delete
 * @return true if the specified node was found and is deleted or false
 */
bool node_hash_del(di_node_hash_t *hash, addr_wpan_t wpan_address);

/**
 * Return whether the hashtable has at least one node
 * @param hash hashtable
 * @return true if there is at least one node in this hashtable, or false if it's empty
 */
bool node_hash_is_empty(di_node_hash_t *hash);

#endif	/* NODE_H */

