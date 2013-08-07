#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "link.h"
#include "../data_collector/rpl_event_callbacks.h"

typedef struct di_link {
	di_link_key_t key;

	di_metric_t metric;
	time_t last_update;		//TX only
	time_t expiration_time;
	uint32_t packet_count;	//TX only

	bool has_changed;
	void *user_data;
} di_link_t;

static void link_set_changed(di_link_t *link);

size_t link_sizeof() {
	return sizeof(di_link_t);
}

void link_init(void *data, const void *key, size_t key_size) {
	di_link_t *link = (di_link_t*) data;

	assert(key_size == sizeof(di_link_ref_t));

	link->key.ref = *(di_link_ref_t*) key;
	link->has_changed = true;
	rpl_event_link_created(link);
}

void link_key_init(di_link_key_t *key, di_node_ref_t child, di_node_ref_t parent, uint32_t version) {
	memset(key, 0, sizeof(di_link_key_t));

	key->ref.child = child;
	key->ref.parent = parent;
}

void link_ref_init(di_link_ref_t *ref, di_node_ref_t child, di_node_ref_t parent) {
	memset(ref, 0, sizeof(di_link_ref_t));

	ref->child = child;
	ref->parent = parent;
}

bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count) {
	link->last_update = time;
	link->packet_count += added_packet_count;
	//link_set_changed(link);

	//No update here, managed by has_changed mechanism

	return true;
}

static void link_set_changed(di_link_t *link) {
	if(link->has_changed == false)
		rpl_event_link_updated(link);
	link->has_changed = true;
}

di_link_t *link_dup(di_link_t *link) {
	di_link_t *new_link;

	new_link = malloc(sizeof(di_link_t));
	memcpy(new_link, link, sizeof(di_link_t));

	return new_link;
}

void link_set_key(di_link_t *link, di_link_key_t *key) {
	if(memcmp(&link->key, key, sizeof(di_link_key_t))) {
		link->key = *key;
		link_set_changed(link);
	}
}

void link_set_metric(di_link_t *link, const di_metric_t *metric) {
	if(link->metric.type != metric->type || link->metric.value != metric->value) {
		link->metric = *metric;
		link_set_changed(link);
	}
}

void link_set_user_data(di_link_t *link, void *user_data) {
	link->user_data = user_data;
}

bool link_has_changed(di_link_t *link) {
	return link->has_changed;
}

void link_reset_changed(di_link_t *link) {
	link->has_changed = false;
}


const di_link_key_t *link_get_key(const di_link_t *link) {
	return &link->key;
}

time_t link_get_last_update(const di_link_t *link) {
	return link->last_update;
}

uint32_t link_get_packet_count(const di_link_t *link) {
	return link->packet_count;
}

const di_metric_t* link_get_metric(const di_link_t *link) {
	return &link->metric;
}

void *link_get_user_data(const di_link_t *link) {
	return link->user_data;
}
