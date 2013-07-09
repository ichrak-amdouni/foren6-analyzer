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

typedef struct di_route_el {
	di_prefix_t route_prefix;
	addr_wpan_t via_node;
	
	void *user_data;

	struct di_route_el *next;
} di_route_el_t, *di_route_list_t;

di_route_el_t *route_get(di_route_list_t *list, di_prefix_t route_prefix, bool allow_summary);
di_route_el_t *route_add(di_route_list_t *list, di_prefix_t route_prefix, bool auto_summary);
bool route_remove(di_route_list_t *list, di_prefix_t route_prefix);
bool route_del_all_outdated(di_route_list_t *list);

di_route_list_t route_dup(di_route_list_t *routes);

#endif	/* ROUTE_H */

