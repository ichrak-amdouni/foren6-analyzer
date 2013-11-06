/*
 * File:   link.h
 * Author: am
 *
 * Created on June 21, 2013, 10:42 AM
 */

#ifndef LINK_H
#define	LINK_H

#include <stdbool.h>
#include <stdint.h>
#include "metric.h"
#include "address.h"
#include "node.h"

#include "link_type.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_link_ref {
	di_node_ref_t child;
	di_node_ref_t parent;
} di_link_ref_t;

typedef struct di_link_key {
	di_link_ref_t ref;
} di_link_key_t;

size_t link_sizeof();

void link_init(void *data, const void *key, size_t key_size);
void link_destroy(void *data);

void link_key_init(di_link_key_t *key, di_node_ref_t child, di_node_ref_t parent, uint32_t version);
void link_ref_init(di_link_ref_t *ref, di_node_ref_t child, di_node_ref_t parent);
di_link_t *link_dup(const di_link_t *link);

void link_set_key(di_link_t *link, di_link_key_t *key);
void link_set_metric(di_link_t *link, const di_metric_t *metric);
void link_set_user_data(di_link_t *link, void *user_data);
bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count);

bool link_has_changed(di_link_t *link);
void link_reset_changed(di_link_t *link);

const di_link_key_t *link_get_key(const di_link_t *link);
time_t link_get_last_update(const di_link_t *link);
uint32_t link_get_packet_count(const di_link_t *link);
bool link_get_deprecated(const di_link_t *link);
const di_metric_t* link_get_metric(const di_link_t *link);
void *link_get_user_data(const di_link_t *link);

void links_deprecate_all_from(di_link_ref_t const *new_link_ref);

#ifdef	__cplusplus
}
#endif

#endif	/* LINK_H */

