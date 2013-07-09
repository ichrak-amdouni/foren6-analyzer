#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "link.h"
#include "../data_collector/rpl_event_callbacks.h"

void link_init(di_link_t *link) {
	link = link;
}

bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count) {
	link->last_update = time;
	link->packet_count += added_packet_count;
	
	rpl_event_link_updated(link);
	
	return true;
}

di_link_t *link_dup(di_link_t *link) {
	di_link_t *new_link;
	
	new_link = malloc(sizeof(di_link_t));
	memcpy(new_link, link, sizeof(di_link_t));
	
	return new_link;
}
