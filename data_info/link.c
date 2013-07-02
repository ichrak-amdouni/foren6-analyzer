#include <stdlib.h>
#include <time.h>

#include "link.h"
#include "../uthash.h"
#include "../data_collector/rpl_event_callbacks.h"

di_link_t *link_hash_get(di_link_hash_t *hash, di_node_t *child_node, di_node_t *parent_node, bool get_or_create) {
	di_link_el_t *link_el;
	di_link_addr_pair_t link_key;
	
	memset(&link_key, 0, sizeof(link_key));
	link_key.child = child_node;
	link_key.parent = parent_node;
	
	HASH_FIND(hh, *hash, &link_key, sizeof(link_key), link_el);
	if(get_or_create && !link_el) {
		link_el = (di_link_el_t*) calloc(1, sizeof(di_link_el_t));
		link_el->link = (di_link_t*) calloc(1, sizeof(di_link_t));
		link_el->link->key = link_key;
		HASH_ADD(hh, *hash, link->key, sizeof(di_link_addr_pair_t), link_el);
		rpl_event_link_created(link_el->link);
	}
	
	return (link_el)? link_el->link : NULL;
}

bool link_hash_del(di_link_hash_t *hash, di_node_t *child_node, di_node_t *parent_node) {
	di_link_el_t *current_link;
	di_link_addr_pair_t link_key;
	
	memset(&link_key, 0, sizeof(link_key));
	link_key.child = child_node;
	link_key.parent = parent_node;

	HASH_FIND(hh, *hash, &link_key, sizeof(di_link_addr_pair_t), current_link);
	if(current_link) {
		rpl_event_link_deleted(current_link->link);
		
		memset(current_link->link, 0, sizeof(di_link_t));
		free(current_link->link);
		HASH_DEL(*hash, current_link);
		memset(current_link, 0, sizeof(di_link_el_t));
		free(current_link);

		return true;
	}
	
	return false;
}

bool link_hash_remove_all_outdated(di_link_hash_t *hash) {
	di_link_el_t *current_link, *tmp;
	bool found_some = false;
	time_t current_time = time(NULL);

	HASH_ITER(hh, *hash, current_link, tmp) {
		if(current_link->link->expiration_time >= current_time) {
			rpl_event_link_deleted(current_link->link);
			memset(current_link->link, 0, sizeof(di_link_t));
			free(current_link->link);
			HASH_DEL(*hash, current_link);
			memset(current_link, 0, sizeof(di_link_el_t));
			free(current_link);
			found_some = true;
		}
	}
	
	return found_some;
}

bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count) {
	link->last_update = time;
	link->packet_count += added_packet_count;
	
	rpl_event_link_updated(link);
	
	return true;
}
