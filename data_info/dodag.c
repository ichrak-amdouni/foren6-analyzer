#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dodag.h"
#include "node.h"
#include "rpl_data.h"

struct di_dodag {
	di_dodag_key_t key;				//Via DIO & DAO for dodagid and via DIO for version

	//Configuration
	di_dodag_config_t config;				//Via DIO config option

	di_prefix_t prefix;						//Via DIO prefix option

	di_rpl_instance_ref_t rpl_instance;		//Via DIO, DAO

	//Nodes
	hash_container_ptr nodes;					//Via DIO, sometimes DAO

	bool has_changed;
	void *user_data;
};

size_t dodag_sizeof() {
	return sizeof(di_dodag_t);
}

void dodag_init(void *data, const void *key, size_t key_size) {
	di_dodag_t *dodag = (di_dodag_t*) data;

	assert(key_size == sizeof(di_dodag_ref_t));

	dodag->nodes = hash_create(sizeof(di_node_ref_t), NULL);
	dodag->rpl_instance.rpl_instance = -1;
	dodag->key.ref = *(di_dodag_ref_t*)key;
	dodag->has_changed = true;
}

di_dodag_t *dodag_dup(di_dodag_t *dodag) {
	di_dodag_t *new_dodag;

	new_dodag = malloc(sizeof(di_dodag_t));
	memcpy(new_dodag, dodag, sizeof(di_dodag_t));
	new_dodag->nodes = hash_dup(dodag->nodes);

	return new_dodag;
}

void dodag_key_init(di_dodag_key_t *key, addr_ipv6_t dodag_id, uint8_t dodag_version, uint32_t version) {
	memset(key, 0, sizeof(di_dodag_key_t));

	key->ref.dodagid = dodag_id;
	key->ref.version = dodag_version;
}

void dodag_ref_init(di_dodag_ref_t *ref, addr_ipv6_t dodag_id, uint8_t dodag_version) {
	memset(ref, 0, sizeof(di_dodag_ref_t));

	ref->dodagid = dodag_id;
	ref->version = dodag_version;
}

void dodag_set_key(di_dodag_t *dodag, const di_dodag_key_t *key) {
	if(memcmp(&dodag->key, key, sizeof(di_dodag_key_t))) {
		dodag->key = *key;
		dodag->has_changed = true;
	}
}

void dodag_set_config(di_dodag_t *dodag, const di_dodag_config_t *config) {
	if(memcmp(&dodag->config, config, sizeof(di_dodag_config_t))) {
		dodag->config = *config;
		dodag->has_changed = true;
	}
}

void dodag_set_prefix(di_dodag_t *dodag, const di_prefix_t *prefix) {
	hash_iterator_ptr it, itend;


	if(dodag->prefix.length == prefix->length && !addr_compare_ip_len(&dodag->prefix.prefix, &prefix->prefix, prefix->length))
		return;	//Same prefix, nothing to change

	dodag->prefix = *prefix;

	it = hash_begin(dodag->nodes, NULL);
	itend = hash_end(dodag->nodes, NULL);

	for(; hash_it_equ(it, itend) == false; hash_it_inc(it)) {
		di_node_t *node = rpldata_get_node(hash_it_value(it), HVM_FailIfNonExistant, NULL);
		assert(node != NULL);
		assert(!memcmp(node_get_dodag(node), &dodag->key.ref, sizeof(di_dodag_ref_t)));
		node_update_ip(node, prefix);
	}

	hash_it_destroy(it);
	hash_it_destroy(itend);

	dodag->has_changed = true;
}

void dodag_set_rpl_instance(di_dodag_t *dodag, const di_rpl_instance_ref_t* rpl_instance) {
	if(dodag->rpl_instance.rpl_instance != rpl_instance->rpl_instance) {
		dodag->rpl_instance = *rpl_instance;
		dodag->has_changed = true;
	}
}

void dodag_add_node(di_dodag_t *dodag, di_node_t *node) {
	bool was_already_in_dodag = false;

	hash_add(dodag->nodes, hash_key_make(node_get_key(node)->ref), &node_get_key(node)->ref, NULL, HAM_OverwriteIfExists, &was_already_in_dodag);

	if(was_already_in_dodag == false) {
		node_set_dodag(node, &dodag->key.ref);
		node_update_ip(node, &dodag->prefix);
		dodag->has_changed = true;
	} else {
		assert(!memcmp(&node_get_dodag(node)->dodagid, &dodag->key.ref.dodagid, sizeof(addr_ipv6_t)));
		if(node_get_dodag(node)->version < dodag->key.ref.version)
			node_set_dodag(node, &dodag->key.ref);
	}
}

void dodag_del_node(di_dodag_t *dodag, di_node_t *node) {
	static const di_dodag_ref_t null_ref = {{{{0}}}, -1};

	if(hash_delete(dodag->nodes, hash_key_make(node_get_key(node)->ref))) {
		node_set_dodag(node, &null_ref);
		dodag->has_changed = true;
	}
}

void dodag_set_user_data(di_dodag_t *dodag, void *user_data) {
	dodag->user_data = user_data;
}

bool dodag_has_changed(di_dodag_t *dodag) {
	return dodag->has_changed;
}

void dodag_reset_changed(di_dodag_t *dodag) {
	dodag->has_changed = false;
}


const di_dodag_key_t *dodag_get_key(const di_dodag_t *dodag) {
	return &dodag->key;
}

const di_dodag_config_t *dodag_get_config(const di_dodag_t *dodag) {
	return &dodag->config;
}

const di_prefix_t *dodag_get_prefix(const di_dodag_t *dodag) {
	return &dodag->prefix;
}

const di_rpl_instance_ref_t *dodag_get_rpl_instance(const di_dodag_t *dodag) {
	return &dodag->rpl_instance;
}

hash_container_ptr dodag_get_node(const di_dodag_t *dodag) {
	return dodag->nodes;
}

void *dodag_get_user_data(const di_dodag_t *dodag) {
	return dodag->user_data;
}

