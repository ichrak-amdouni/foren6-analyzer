#include "dodag.h"
#include <stdio.h>

#include "../data_collector/rpl_event_callbacks.h"

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
di_dodag_t *dodag_hash_get(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version, bool get_or_create) {
	di_dodag_el_t *current_dodag;
	di_dodag_key_t key;	
	
	memset(&key, 0, sizeof(key));	//Padding in structure must be always 0
	key.dodagid = *dodagid;
	key.version = version;

	HASH_FIND(hh, *hash, &key, sizeof(di_dodag_key_t), current_dodag);
	if(get_or_create && !current_dodag) {
		/* dodag was not known before, so add it to known dodags */
		current_dodag = (di_dodag_el_t*) calloc(1, sizeof(di_dodag_el_t));
		current_dodag->dodag = (di_dodag_t*) calloc(1, sizeof(di_dodag_t));
		current_dodag->dodag->dodag_key = key;
		HASH_ADD(hh, *hash, dodag->dodag_key, sizeof(di_dodag_key_t), current_dodag);
		rpl_event_dodag_created(current_dodag->dodag);
	}
	
	return (current_dodag)? current_dodag->dodag : NULL;
}

/**
 * Add an existing dodag structure to the hashtable.
 * @param hash hashtable
 * @param dodag pointer to the dodag structure to add
 * @param overwrite_existing if the dodag was already in the hashtable but with a different pointer, overwrite it only if this parameter is true (the old di_dodag_t structure is not freed)
 * @return the dodag represented by dodagid and version in this hashtable. May not be the dodag parameter if overwrite_existing is false and if there was already existing dodag with same id and version.
 */
di_dodag_t *dodag_hash_add(di_dodag_hash_t *hash, di_dodag_t *dodag, bool overwrite_existing) {
	di_dodag_el_t *current_dodag;

	HASH_FIND(hh, *hash, &dodag->dodag_key, sizeof(di_dodag_key_t), current_dodag);
	if(overwrite_existing && current_dodag) {
		fprintf(stderr, "DODAG hashtable: overwrite of a dodag !\n");
		rpl_event_dodag_deleted(current_dodag->dodag);
		HASH_DEL(*hash, current_dodag);
		free(current_dodag->dodag);
		free(current_dodag);
		current_dodag = NULL;
	}
	if(!current_dodag) {
		/* dodag was not known before, so add it to known dodags */
		current_dodag = (di_dodag_el_t*) calloc(1, sizeof(di_dodag_el_t));
		current_dodag->dodag = dodag;
		HASH_ADD(hh, *hash, dodag->dodag_key, sizeof(di_dodag_key_t), current_dodag);
	}
	
	return current_dodag->dodag;
}

/**
 * Remove a dodag structure from the hashtable.
 * The dodag structure is not freed.
 * 
 * @param hash hashtable
 * @param dodagid the dodagid of the dodag to remove
 * @param version the version of the dodag to remove
 * @return a pointer to the removed dodag structure or NULL if it was not found
 */
di_dodag_t *dodag_hash_remove(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version) {
	di_dodag_el_t *current_dodag;
	di_dodag_key_t key;
	di_dodag_t *old_dodag = NULL;
	
	key.dodagid = *dodagid;
	key.version = version;

	HASH_FIND(hh, *hash, &key, sizeof(di_dodag_key_t), current_dodag);
	if(current_dodag) {
		old_dodag = current_dodag->dodag;

		HASH_DEL(*hash, current_dodag);
		free(current_dodag);
	}
	
	return old_dodag;
}

/**
 * Remove a dodag from the hashtable and free its memory
 * If the dodag exist in the hashtable, this function is equivalent to free(dodag_hash_remove(...))
 * @param hash hashtable
 * @param dodagid the dodagid of the dodag to delete
 * @param version the version of the dodag to delete
 * @return true if the specified dodag was found and is deleted or false
 */
bool dodag_hash_del(di_dodag_hash_t *hash, addr_ipv6_t *dodagid, uint16_t version) {
	di_dodag_t *dodag;
	
	dodag = dodag_hash_remove(hash, dodagid, version);
	if(dodag) {
		rpl_event_dodag_deleted(dodag);
		free(dodag);
		return true;
	} else return false;
}

/**
 * Return whether the hashtable has at least one dodag
 * @param hash hashtable
 * @return true if there is at least one dodag in this hashtable, or false if it's empty
 */
bool dodag_hash_is_empty(di_dodag_hash_t *hash) {
	return (HASH_COUNT(*hash)) == 0;
}
