/* 
 * File:   dodag.h
 * Author: am
 *
 * Created on June 20, 2013, 5:04 PM
 */

#ifndef DODAG_H
#define	DODAG_H

#include "node.h"
#include "../sniffer_packet_parser.h"

#include "address.h"
#include "node.h"
#include "rpl_instance.h"
#include "../uthash.h"

typedef struct di_dodag_el *di_dodag_hash_t;
typedef struct di_node_el *di_node_hash_t;

typedef struct di_dodag_key {
	addr_ipv6_t dodagid;				//Via DIO, DAO
	uint16_t version;					//Via DIO
} di_dodag_key_t;

typedef struct di_dodag {
	di_rpl_instance_t *rpl_instance;		//Via DIO, DAO
	
	di_dodag_key_t dodag_key;				//Via DIO & DAO for dodagid and via DIO for version
	
	//Configuration
	rpl_dodag_config_t config;				//Via DIO config option

	di_prefix_t prefix;						//Via DIO prefix option
	
	//Nodes
	di_node_hash_t nodes;					//Via DIO, sometimes DAO
	
	void *user_data;
} di_dodag_t;

typedef struct di_dodag_el {
	di_dodag_t *dodag;
    UT_hash_handle hh;
} di_dodag_el_t, *di_dodag_hash_t;

/**
 * Get the dodag with specified dodagid and version in the hashtable;
 * If it does not exist, create it if get_or_create is true, else return NULL.
 * 
 * @param hash hashtable from where to get the dodag
 * @param dodagid the id of the dodag to search
 * @param version the version of the dodag
 * @param get_or_create if true, if the dodag is not found in the hashtable, it will be created. If false, NULL is returned when the dodag is not found
 * @return the dodag structure containing data for the specified id and version
 */
di_dodag_t *dodag_hash_get(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version, bool get_or_create);

/**
 * Add an existing dodag structure to the hashtable.
 * @param hash hashtable
 * @param dodag pointer to the dodag structure to add
 * @param overwrite_existing if the dodag was already in the hashtable but with a different pointer, overwrite it only if this parameter is true
 * @return the dodag represented by dodagid and version in this hashtable. May not be the dodag parameter if overwrite_existing is false and if there was already existing dodag with same id and version.
 */
di_dodag_t *dodag_hash_add(di_dodag_hash_t *hash, di_dodag_t *dodag, bool overwrite_existing);

/**
 * Remove a dodag structure from the hashtable.
 * The dodag structure is not freed.
 * 
 * @param hash hashtable
 * @param dodagid the dodagid of the dodag to remove
 * @param version the version of the dodag to remove
 * @return a pointer to the removed dodag structure or NULL if it was not found
 */
di_dodag_t *dodag_hash_remove(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version);

/**
 * Remove a dodag from the hashtable and free its memory
 * If the dodag exist in the hashtable, this function is equivalent to free(dodag_hash_remove(...))
 * @param hash hashtable
 * @param dodagid the dodagid of the dodag to delete
 * @param version the version of the dodag to delete
 * @return true if the specified dodag was found and is deleted or false
 */
bool dodag_hash_del(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version);

/**
 * Return whether the hashtable has at least one dodag
 * @param hash hashtable
 * @return true if there is at least one dodag in this hashtable, or false if it's empty
 */
bool dodag_hash_is_empty(di_dodag_hash_t *hash);

#endif	/* DODAG_H */

