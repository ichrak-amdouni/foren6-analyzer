#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dodag.h"
#include "node.h"
#include "rpl_data.h"

typedef struct di_dodag {
	di_dodag_key_t key;				//Via DIO & DAO for dodagid and via DIO for version

	//Configuration
	di_dodag_config_t config;				//Via DIO config option

	di_prefix_t prefix;						//Via DIO prefix option

	di_rpl_instance_ref_t rpl_instance;		//Via DIO, DAO

	//Nodes
	hash_container_ptr nodes;					//Via DIO, sometimes DAO

	void *user_data;
} di_dodag_t;

size_t dodag_sizeof() {
	return sizeof(di_dodag_t);
}

void dodag_init(void *data, void *key, size_t key_size) {
	di_dodag_t *dodag = (di_dodag_t*) data;

	assert(key_size == sizeof(di_dodag_key_t));

	dodag->nodes = hash_create(sizeof(di_node_ref_t), NULL);
	dodag->rpl_instance.rpl_instance = -1;
	dodag->key = *(di_dodag_key_t*)key;
}

di_dodag_t *dodag_dup(di_dodag_t *dodag) {
	di_dodag_t *new_dodag;

	new_dodag = malloc(sizeof(di_dodag_t));
	memcpy(new_dodag, dodag, sizeof(di_dodag_t));
	new_dodag->nodes = hash_dup(dodag->nodes);

	return new_dodag;
}

void dodag_set_key(di_dodag_t *dodag, const di_dodag_key_t *key) {
	dodag->key = *key;
}

void dodag_set_config(di_dodag_t *dodag, const di_dodag_config_t *config) {
	dodag->config = *config;
}

void dodag_set_prefix(di_dodag_t *dodag, const di_prefix_t *prefix) {
	hash_iterator_ptr it, itend;

/*
	if(dodag->prefix.length == prefix->length && !addr_compare_ip_len(&dodag->prefix.prefix, &prefix->prefix, prefix->length))
		return;	//Same prefix, nothing to change
*/

	dodag->prefix = *prefix;

	it = hash_begin(dodag->nodes, NULL);
	itend = hash_end(dodag->nodes, NULL);

	for(; hash_it_equ(it, itend) == false; hash_it_inc(it)) {
		di_node_key_t node_key = {*(di_node_ref_t*)hash_it_value(it), 0};
		di_node_t *node = rpldata_get_node(&node_key);
		assert(node != NULL);
		assert(!memcmp(node_get_dodag(node), &dodag->key.ref, sizeof(di_dodag_ref_t)));
		node_update_ip(node, prefix);
	}

	hash_it_destroy(it);
	hash_it_destroy(itend);
}

void dodag_set_rpl_instance(di_dodag_t *dodag, const di_rpl_instance_ref_t* rpl_instance) {
	dodag->rpl_instance = *rpl_instance;
}

void dodag_add_node(di_dodag_t *dodag, di_node_t *node) {
	bool was_already_in_dodag = false;

	hash_add(dodag->nodes, hash_key_make(node_get_key(node)->ref), &node_get_key(node)->ref, NULL, HAM_OverwriteIfExists, &was_already_in_dodag);

	if(was_already_in_dodag == false) {
		node_set_dodag(node, &dodag->key.ref);
		node_update_ip(node, &dodag->prefix);
	} else {
		assert(!memcmp(node_get_dodag(node), &dodag->key.ref, sizeof(di_dodag_ref_t)));
	}
}

void dodag_del_node(di_dodag_t *dodag, di_node_t *node) {
	static const di_dodag_ref_t null_ref = {{{{0}}}, -1};

	hash_delete(dodag->nodes, hash_key_make(node_get_key(node)->ref));
	node_set_dodag(node, &null_ref);
}

void dodag_set_user_data(di_dodag_t *dodag, void *user_data) {
	dodag->user_data = user_data;
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

