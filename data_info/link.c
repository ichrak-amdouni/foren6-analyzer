#include <time.h>

#include "link.h"
#include "../data_collector/rpl_event_callbacks.h"

bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count) {
	link->last_update = time;
	link->packet_count += added_packet_count;
	
	rpl_event_link_updated(link);
	
	return true;
}
