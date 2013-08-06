#include "rpl_packet_parser.h"
#include "descriptor_poll.h"
#include "interface_reader/interfaces_register.h"
#include "sniffer_packet_parser.h"

void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks) {
	rpl_event_set_callbacks(callbacks);
}

void rpl_tool_init() {
	desc_poll_init();
	rpldata_init();
	sniffer_parser_init();
}

void rpl_tool_cleanup() {
	desc_poll_cleanup();
}

interface_t *rpl_tool_get_interface(const char* name) {
	return interface_get(name);
}
