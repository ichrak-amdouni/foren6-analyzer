#include "node.h"
#include "dodag.h"
#include <stdio.h>

#include "../data_collector/rpl_event_callbacks.h"

/**
 * Get the node with specified address in the hashtable;
 * If it does not exist, create it if get_or_create is true, else return NULL.
 * 
 * @param hash hashtable from where to get the node
 * @param wpan_address the address of the node to search
 * @param get_or_create if true, if the node is not found in the hashtable, it will be created. If false, NULL is returned when the node is not found
 * @return the node structure containing data for the specified id and version
 */
di_node_t *node_hash_get(di_node_hash_t *hash, addr_wpan_t wpan_address, bool get_or_create) {
	di_node_el_t *current_node;

	HASH_FIND(hh, *hash, &wpan_address, sizeof(addr_wpan_t), current_node);
	if(get_or_create && !current_node) {
		current_node = (di_node_el_t*) calloc(1, sizeof(di_node_el_t));
		current_node->node = (di_node_t*) calloc(1, sizeof(di_node_t));
		current_node->node->wpan_address = wpan_address;
		current_node->node->local_address = addr_get_local_ip_from_mac64(wpan_address);
		HASH_ADD(hh, *hash, node->wpan_address, sizeof(addr_wpan_t), current_node);
		rpl_event_node_created(current_node->node);
	}
	
	return (current_node)? current_node->node : NULL;
}

/**
 * Add an existing node structure to the hashtable.
 * @param hash hashtable
 * @param node pointer to the node structure to add
 * @param overwrite_existing if the node was already in the hashtable but with a different pointer, overwrite it only if this parameter is true
 * @return the node in the hashtable. May not be the node parameter if overwrite_existing is false and if there was already existing node with same address.
 */
di_node_t *node_hash_add(di_node_hash_t *hash, di_node_t *node, bool overwrite_existing) {
	di_node_el_t *current_node;

	HASH_FIND(hh, *hash, &node->wpan_address, sizeof(addr_wpan_t), current_node);
	if(overwrite_existing && current_node) {
		fprintf(stderr, "Node hashtable: overwrite of a node !\n");
		rpl_event_node_deleted(current_node->node);
		HASH_DEL(*hash, current_node);
		free(current_node->node);
		free(current_node);
		current_node = NULL;
	}
	if(!current_node) {
		/* dodag was not known before, so add it to known dodags */
		current_node = (di_node_el_t*) calloc(1, sizeof(di_node_el_t));
		current_node->node = node;
		HASH_ADD(hh, *hash, node->wpan_address, sizeof(addr_wpan_t), current_node);
	}
	
	return current_node->node;
}

/**
 * Remove a node structure from the hashtable.
 * The node structure is not freed.
 * 
 * @param hash hashtable
 * @param wpan_address the address of the node to remove
 * @return a pointer to the removed node structure or NULL if it was not found
 */
di_node_t *node_hash_remove(di_node_hash_t *hash, uint64_t wpan_address) {
	di_node_el_t *current_node;
	di_node_t *old_node = NULL;

	HASH_FIND(hh, *hash, &wpan_address, sizeof(addr_wpan_t), current_node);
	if(current_node) {
		old_node = current_node->node;

		HASH_DEL(*hash, current_node);
		free(current_node);
	}
	
	return old_node;
}

/**
 * Remove a node from the hashtable and free its memory
 * If the node exist in the hashtable, this function is equivalent to free(node_hash_remove(...))
 * @param hash hashtable
 * @param wpan_address the address of the node to delete
 * @return true if the specified node was found and is deleted or false
 */
bool node_hash_del(di_node_hash_t *hash, uint64_t wpan_address) {
	di_node_t *node;
	
	node = node_hash_remove(hash, wpan_address);
	if(node) {
		rpl_event_node_deleted(node);
		free(node);
		return true;
	} else return false;
}

/**
 * Return whether the hashtable has at least one node
 * @param hash hashtable
 * @return true if there is at least one node in this hashtable, or false if it's empty
 */
bool node_hash_is_empty(di_node_hash_t *hash) {
	return (HASH_CNT(hh, *hash)) == 0;
}
