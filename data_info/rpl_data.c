#include <assert.h>

#include "rpl_data.h"
#include "../utlist.h"
#include <stdio.h>
#include <stdlib.h>

#include "link.h"
#include "node.h"
#include "dodag.h"
#include "rpl_instance.h"

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

typedef struct di_rpl_wsn_state {
	uint32_t node_version;
	uint32_t dodag_version;
	uint32_t rpl_instance_version;
	uint32_t links_version;
} di_rpl_wsn_state_t;

typedef struct di_rpl_object_el {
	void *object;
	struct di_rpl_object_el* next;
	struct di_rpl_object_el* prev;
} di_rpl_object_el_t, *di_rpl_object_list_t;

typedef struct {
	di_rpl_object_list_t nodes;
	di_rpl_object_list_t dodags;
	di_rpl_object_list_t rpl_instances;
	di_rpl_object_list_t links;
} di_rpl_allocated_objects_t;

di_rpl_allocated_objects_t allocated_objects;

di_rpl_wsn_state_t wsn_versions[20000];
uint32_t wsn_last_version = 0;


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

	wsn_versions[0].node_version = 0;
	wsn_versions[0].dodag_version = 0;
	wsn_versions[0].rpl_instance_version = 0;
	wsn_versions[0].links_version = 0;
}

di_rpl_data_t *rpldata_get() {
	return &collected_data;
}

hash_container_ptr rpldata_get_nodes(uint32_t version) {
	if(wsn_versions[version].node_version == -1)
		return NULL;

	hash_container_ptr *ptr = hash_value(collected_data.nodes, hash_key_make(wsn_versions[version].node_version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_dodags(uint32_t version) {
	if(wsn_versions[version].dodag_version == -1)
		return NULL;

	hash_container_ptr *ptr = hash_value(collected_data.dodags, hash_key_make(wsn_versions[version].dodag_version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_rpl_instances(uint32_t version) {
	if(wsn_versions[version].rpl_instance_version == -1)
		return NULL;

	hash_container_ptr *ptr = hash_value(collected_data.rpl_instances, hash_key_make(wsn_versions[version].rpl_instance_version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}

hash_container_ptr rpldata_get_links(uint32_t version) {
	if(wsn_versions[version].links_version == -1)
		return NULL;

	hash_container_ptr *ptr = hash_value(collected_data.links, hash_key_make(wsn_versions[version].links_version), HVM_FailIfNonExistant, NULL);
	if(ptr)
		return *ptr;
	else return NULL;
}


uint32_t rpldata_add_node_version() {
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

	return new_version;
}

uint32_t rpldata_add_dodag_version() {
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

	return new_version;
}

uint32_t rpldata_add_rpl_instance_version() {
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

	return new_version;
}

uint32_t rpldata_add_link_version() {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	hash_iterator_ptr itEnd = hash_begin(NULL, NULL);
	link_last_version++;
	uint32_t new_version = link_last_version;

	hash_container_ptr new_version_container = hash_create(link_sizeof(), &link_init);
	hash_container_ptr working_container = rpldata_get_links(0);

	for(hash_begin(working_container, it), hash_end(working_container, itEnd); !hash_it_equ(it, itEnd); hash_it_inc(it)) {
		di_link_t *link = hash_it_value(it);
		di_link_key_t link_key = {link_get_key(link)->ref, new_version};
		di_link_t *new_link = link_dup(link);
		link_set_key(new_link, &link_key);
		hash_add(new_version_container, hash_key_make(link_key.ref), new_link, NULL, HAM_NoCheck, NULL);
	}

	hash_add(collected_data.links, hash_key_make(new_version), &new_version_container, NULL, HAM_NoCheck, NULL);

	hash_it_destroy(it);
	hash_it_destroy(itEnd);

	return new_version;
}

di_node_t *rpldata_get_node(const di_node_ref_t *node_ref, hash_value_mode_e value_mode, bool *was_already_existing) {
	bool already_existing;
	di_node_t *new_node;

	if(was_already_existing == NULL)
		was_already_existing = &already_existing;

	if((new_node = hash_value(rpldata_get_nodes(0), hash_key_make(*node_ref), HVM_FailIfNonExistant, was_already_existing)) == NULL) {
		new_node = calloc(1, node_sizeof());
		node_init(new_node, node_ref, sizeof(*node_ref));

		di_rpl_object_el_t *node_el = calloc(1, sizeof(di_rpl_object_el_t));
		node_el->object = new_node;

		DL_APPEND(allocated_objects.nodes, node_el);
		hash_add(rpldata_get_nodes(0), hash_key_make(*node_ref), new_node, NULL, HAM_NoCheck, NULL);
	}

	return new_node;
}

di_dodag_t *rpldata_get_dodag(const di_dodag_ref_t *dodag_ref, hash_value_mode_e value_mode, bool *was_already_existing) {
	bool already_existing;
	di_dodag_t *new_dodag;

	if(was_already_existing == NULL)
		was_already_existing = &already_existing;

	if((new_dodag = hash_value(rpldata_get_dodags(0), hash_key_make(*dodag_ref), HVM_FailIfNonExistant, was_already_existing)) == NULL) {
		new_dodag = calloc(1, dodag_sizeof());
		dodag_init(new_dodag, dodag_ref, sizeof(*dodag_ref));

		di_rpl_object_el_t *dodag_el = calloc(1, sizeof(di_rpl_object_el_t));
		dodag_el->object = new_dodag;

		DL_APPEND(allocated_objects.dodags, dodag_el);
		hash_add(rpldata_get_dodags(0), hash_key_make(*dodag_ref), new_dodag, NULL, HAM_NoCheck, NULL);
	}

	return new_dodag;
}

di_rpl_instance_t *rpldata_get_rpl_instance(const di_rpl_instance_ref_t *rpl_instance_ref, hash_value_mode_e value_mode, bool *was_already_existing) {
	bool already_existing;
	di_rpl_instance_t *new_rpl_instance;

	if(was_already_existing == NULL)
		was_already_existing = &already_existing;

	if((new_rpl_instance = hash_value(rpldata_get_rpl_instances(0), hash_key_make(*rpl_instance_ref), HVM_FailIfNonExistant, was_already_existing)) == NULL) {
		new_rpl_instance = calloc(1, rpl_instance_sizeof());
		rpl_instance_init(new_rpl_instance, rpl_instance_ref, sizeof(*rpl_instance_ref));

		di_rpl_object_el_t *rpl_instance_el = calloc(1, sizeof(di_rpl_object_el_t));
		rpl_instance_el->object = new_rpl_instance;

		DL_APPEND(allocated_objects.rpl_instances, rpl_instance_el);
		hash_add(rpldata_get_rpl_instances(0), hash_key_make(*rpl_instance_ref), new_rpl_instance, NULL, HAM_NoCheck, NULL);
	}

	return new_rpl_instance;
}

di_link_t *rpldata_get_link(const di_link_ref_t *link_ref, hash_value_mode_e value_mode, bool *was_already_existing) {
	bool already_existing;
	di_link_t *new_link;

	if(was_already_existing == NULL)
		was_already_existing = &already_existing;

	if((new_link = hash_value(rpldata_get_links(0), hash_key_make(*link_ref), HVM_FailIfNonExistant, was_already_existing)) == NULL) {
		new_link = calloc(1, link_sizeof());
		link_init(new_link, link_ref, sizeof(*link_ref));

		di_rpl_object_el_t *link_el = calloc(1, sizeof(di_rpl_object_el_t));
		link_el->object = new_link;

		DL_APPEND(allocated_objects.links, link_el);
		hash_add(rpldata_get_links(0), hash_key_make(*link_ref), new_link, NULL, HAM_NoCheck, NULL);
	}

	return new_link;
}

void rpldata_wsn_create_version() {
	wsn_last_version++;
	if(node_last_version)
		wsn_versions[wsn_last_version].node_version = node_last_version;
	else wsn_versions[wsn_last_version].node_version = -1;

	if(dodag_last_version)
		wsn_versions[wsn_last_version].dodag_version = dodag_last_version;
	else wsn_versions[wsn_last_version].dodag_version = -1;

	if(rpl_instance_last_version)
		wsn_versions[wsn_last_version].rpl_instance_version = rpl_instance_last_version;
	else wsn_versions[wsn_last_version].rpl_instance_version = -1;

	if(link_last_version)
		wsn_versions[wsn_last_version].links_version = link_last_version;
	else wsn_versions[wsn_last_version].links_version = -1;
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

uint32_t rpldata_get_wsn_last_version() {
	return wsn_last_version;
}

