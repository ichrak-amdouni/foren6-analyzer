/*
 * File:   route.h
 * Author: am
 *
 * Created on June 21, 2013, 11:37 AM
 */

#ifndef ROUTE_H
#define	ROUTE_H

#include <stdbool.h>
#include "address.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_route_el {
	di_prefix_t route_prefix;
	addr_wpan_t via_node;

	void *user_data;

	struct di_route_el *next;
} di_route_el_t, *di_route_list_t;

di_route_el_t *route_get(di_route_list_t *list, di_prefix_t route_prefix, addr_wpan_t via_node, bool allow_summary);
di_route_el_t *route_add(di_route_list_t *list, di_prefix_t route_prefix, addr_wpan_t via_node, bool auto_summary, bool *was_already_existing);
bool route_remove(di_route_list_t *list, di_prefix_t route_prefix, addr_wpan_t via_node);
bool route_del_all_outdated(di_route_list_t *list);

di_route_list_t route_dup(const di_route_list_t *routes);
void route_destroy(di_route_list_t *routes);

#ifdef	__cplusplus
}
#endif

#endif	/* ROUTE_H */

