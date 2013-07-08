#include "rpl_packet_parser.h"
#include "descriptor_poll.h"
#include "interface_reader/interfaces_register.h"
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks) {
	rpl_event_set_callbacks(callbacks);
}


void rpl_tool_init(const char *name, const char *target) {
	desc_poll_init();
	interface_register_all();
	rpldata_init();
	sniffer_parser_init();
}

interface_t *rpl_tool_get_interface(const char* name) {
	return interface_get(name);
}

rpl_collector_t *rpl_tool_get_collected_data() {
	return rpldata_get();
}
