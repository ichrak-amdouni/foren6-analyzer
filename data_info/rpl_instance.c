#include "rpl_instance.h"
#include <stdio.h>

#include "../data_collector/rpl_event_callbacks.h"

di_rpl_instance_t *rpl_instance_get(di_rpl_instance_hash_t *hash, uint16_t instance_id, bool get_or_create) {
	di_rpl_instance_el_t *current_instance;

	HASH_FIND(hh, *hash, &instance_id, sizeof(uint16_t), current_instance);
	if(get_or_create && !current_instance) {
		current_instance = (di_rpl_instance_el_t*) calloc(1, sizeof(di_rpl_instance_el_t));
		current_instance->rpl_instance = (di_rpl_instance_t*) calloc(1, sizeof(di_rpl_instance_t));
		current_instance->rpl_instance->instance_id = instance_id;
		HASH_ADD(hh, *hash, rpl_instance->instance_id, sizeof(uint16_t), current_instance);
		rpl_event_rpl_instance_created(current_instance->rpl_instance);
	}
	
	return (current_instance)? current_instance->rpl_instance : NULL;
}

bool rpl_instance_add(di_rpl_instance_hash_t *hash, di_rpl_instance_t *rpl_instance, bool overwrite_existing) {
	di_rpl_instance_el_t *current_instance;

	HASH_FIND(hh, *hash, &rpl_instance->instance_id, sizeof(uint16_t), current_instance);
	if(overwrite_existing && current_instance) {
		fprintf(stderr, "RPL instance hashtable: overwrite of an instance !\n");
		rpl_event_rpl_instance_deleted(current_instance->rpl_instance);
		HASH_DEL(*hash, current_instance);
		free(current_instance->rpl_instance);
		free(current_instance);
		current_instance = NULL;
	}
	if(!current_instance) {
		current_instance = (di_rpl_instance_el_t*) calloc(1, sizeof(di_rpl_instance_el_t));
		current_instance->rpl_instance = rpl_instance;
		HASH_ADD(hh, *hash, rpl_instance->instance_id, sizeof(uint16_t), current_instance);
	}
	
	return current_instance->rpl_instance;
}

bool rpl_instance_remove(di_rpl_instance_hash_t *hash, uint16_t instance_id) {
	di_rpl_instance_el_t *current_instance;
	di_rpl_instance_t *old_instance = NULL;

	HASH_FIND(hh, *hash, &instance_id, sizeof(uint16_t), current_instance);
	if(current_instance) {
		old_instance = current_instance->rpl_instance;

		HASH_DEL(*hash, current_instance);
		free(current_instance);
	}
	
	return old_instance;
}
