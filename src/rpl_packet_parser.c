#include "rpl_packet_parser.h"
#include "descriptor_poll.h"
#include "interface_reader/interfaces_mgr.h"
#include "sniffer_packet_parser.h"
#include <stdio.h>

static analyzer_callbacks_t analyzer_callbacks = {0};

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
	return interfacemgr_get(name);
}

void rpl_tool_set_analyzer_callbacks(analyzer_callbacks_t *callbacks) {
    analyzer_callbacks = *callbacks;
}

void rpl_tool_report_error(char const* error_message) {
    fprintf(stderr, "%s\n", error_message);
    if (analyzer_callbacks.onErrorEvent) analyzer_callbacks.onErrorEvent(error_message);
}
