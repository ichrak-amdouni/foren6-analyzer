#include <assert.h>

#include "rpl_data.h"
#include <stdio.h>

di_rpl_data_t collected_data;

uint32_t node_last_version = 0;
uint32_t dodag_last_version = 0;
uint32_t rpl_instance_last_version = 0;
uint32_t link_last_version = 0;

typedef struct di_rpl_data {
	hash_container_ptr nodes;
	hash_container_ptr dodags;
	hash_container_ptr rpl_instances;
	hash_container_ptr links;
} di_rpl_data_t;


void rpldata_init() {
	collected_data.nodes = hash_create(sizeof(hash_container_ptr), NULL);
	collected_data.dodags = hash_create(sizeof(hash_container_ptr), NULL);
	collected_data.rpl_instances = hash_create(sizeof(hash_container_ptr), NULL);
	collected_data.links = hash_create(sizeof(hash_container_ptr), NULL);

	uint32_t working_version = 0;
	hash_container_ptr hash_ptr;

	hash_ptr = hash_create(node_sizeof(), &node_init);
	hash_add(collected_data.nodes, hash_key_make(working_version), &hash_ptr, NULL, HAM_NoCheck, NULL);

	hash_ptr = hash_create(dodag_sizeof(), &dodag_init);
	hash_add(collected_data.dodags, hash_key_make(working_version), &hash_ptr, NULL, HAM_NoCheck, NULL);

	hash_ptr = hash_create(rpl_instance_sizeof(), &rpl_instance_init);
	hash_add(collected_data.rpl_instances, hash_key_make(working_version), &hash_ptr, NULL, HAM_NoCheck, NULL);

	hash_ptr = hash_create(link_sizeof(), &link_init);
	hash_add(collected_data.links, hash_key_make(working_version), &hash_ptr, NULL, HAM_NoCheck, NULL);
}

di_rpl_data_t *rpldata_get() {
	return &collected_data;
}

hash_container_ptr rpldata_get_nodes(uint32_t version) {
	hash_container_ptr *ptr = hash_value(collected_data.nodes, hash_key_make(version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_dodags(uint32_t version) {
	hash_container_ptr *ptr = hash_value(collected_data.dodags, hash_key_make(version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_rpl_instances(uint32_t version) {
	hash_container_ptr *ptr = hash_value(collected_data.rpl_instances, hash_key_make(version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_links(uint32_t version) {
	hash_container_ptr *ptr = hash_value(collected_data.links, hash_key_make(version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}


void rpldata_add_node_version() {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	hash_iterator_ptr itEnd = hash_begin(NULL, NULL);
	node_last_version++;
	uint32_t new_version = node_last_version;

	hash_container_ptr new_version_container = hash_create(node_sizeof(), &node_init);
	hash_container_ptr working_container = rpldata_get_nodes(0);

	for(hash_begin(working_container, it), hash_end(working_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
		di_node_t *node = hash_it_value(it);
		di_node_key_t node_key = {node_get_key(node)->ref, new_version};
		di_node_t *new_node = node_dup(node);
		node_set_key(new_node, &node_key);
		hash_add(new_version_container, hash_key_make(node_key.ref), new_node, NULL, HAM_NoCheck, NULL);
	}

	hash_add(collected_data.nodes, hash_key_make(new_version), &new_version_container, NULL, HAM_NoCheck, NULL);

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

void rpldata_add_dodag_version() {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	hash_iterator_ptr itEnd = hash_begin(NULL, NULL);
	dodag_last_version++;
	uint32_t new_version = dodag_last_version;

	hash_container_ptr new_version_container = hash_create(dodag_sizeof(), &dodag_init);
	hash_container_ptr working_container = rpldata_get_dodags(0);

	for(hash_begin(working_container, it), hash_end(working_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
		di_dodag_t *dodag = hash_it_value(it);
		di_dodag_key_t dodag_key = {dodag_get_key(dodag)->ref, new_version};
		di_dodag_t *new_dodag = dodag_dup(dodag);
		dodag_set_key(new_dodag, &dodag_key);
		hash_add(new_version_container, hash_key_make(dodag_key.ref), new_dodag, NULL, HAM_NoCheck, NULL);
	}

	hash_add(collected_data.dodags, hash_key_make(new_version), &new_version_container, NULL, HAM_NoCheck, NULL);

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

void rpldata_add_rpl_instance_version() {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	hash_iterator_ptr itEnd = hash_begin(NULL, NULL);
	rpl_instance_last_version++;
	uint32_t new_version = rpl_instance_last_version;

	hash_container_ptr new_version_container = hash_create(rpl_instance_sizeof(), &rpl_instance_init);
	hash_container_ptr working_container = rpldata_get_rpl_instances(0);

	for(hash_begin(working_container, it), hash_end(working_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
		di_rpl_instance_t *rpl_instance = hash_it_value(it);
		di_rpl_instance_key_t rpl_instance_key = {rpl_instance_get_key(rpl_instance)->ref, new_version};
		di_rpl_instance_t *new_rpl_instance = rpl_instance_dup(rpl_instance);
		rpl_instance_set_key(new_rpl_instance, &rpl_instance_key);
		hash_add(new_version_container, hash_key_make(rpl_instance_key.ref), new_rpl_instance, NULL, HAM_NoCheck, NULL);
	}

	hash_add(collected_data.rpl_instances, hash_key_make(new_version), &new_version_container, NULL, HAM_NoCheck, NULL);

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

void rpldata_add_link_version() {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	hash_iterator_ptr itEnd = hash_begin(NULL, NULL);
	link_last_version++;
	uint32_t new_version = link_last_version;

	hash_container_ptr new_version_container = hash_create(link_sizeof(), &link_init);
	hash_container_ptr working_container = rpldata_get_links(0);

	for(hash_begin(working_container, it), hash_end(working_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
		di_link_t *link = hash_it_value(it);
		di_link_key_t link_key = {link->key.ref, new_version};
		di_link_t *new_link = link_dup(link);
		new_link->key = link_key;
		hash_add(new_version_container, hash_key_make(link_key.ref), new_link, NULL, HAM_NoCheck, NULL);
	}

	hash_add(collected_data.links, hash_key_make(new_version), &new_version_container, NULL, HAM_NoCheck, NULL);

	hash_it_destroy(it);
	hash_it_destroy(itEnd);
}

di_node_t *rpldata_get_node(const di_node_key_t *node_key) {
	return hash_value(rpldata_get_nodes(node_key->version), hash_key_make(node_key->ref), HVM_FailIfNonExistant, NULL);
}

di_dodag_t *rpldata_get_dodag(const di_dodag_key_t *dodag_key) {
	return hash_value(rpldata_get_dodags(dodag_key->version), hash_key_make(dodag_key->ref), HVM_FailIfNonExistant, NULL);
}

di_rpl_instance_t *rpldata_get_rpl_instance(const di_rpl_instance_key_t *rpl_instance_key) {
	return hash_value(rpldata_get_rpl_instances(rpl_instance_key->version), hash_key_make(rpl_instance_key->ref), HVM_FailIfNonExistant, NULL);
}

di_link_t *rpldata_get_link(const di_link_key_t *link_key) {
	return hash_value(rpldata_get_links(link_key->version), hash_key_make(link_key->ref), HVM_FailIfNonExistant, NULL);
}

void rpldata_create_version() {
	rpldata_add_node_version();
	rpldata_add_dodag_version();
	rpldata_add_rpl_instance_version();
	rpldata_add_link_version();
}

uint32_t rpldata_get_node_last_version() {
	return node_last_version;
}

uint32_t rpldata_get_dodag_last_version() {
	return dodag_last_version;
}

uint32_t rpldata_get_rpl_instance_last_version() {
	return rpl_instance_last_version;
}

uint32_t rpldata_get_link_last_version() {
	return link_last_version;
}

