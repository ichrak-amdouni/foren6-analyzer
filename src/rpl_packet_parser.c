#include "rpl_packet_parser.h"
#include "descriptor_poll.h"
#include "interface_reader/interfaces_mgr.h"
#include "sniffer_packet_parser.h"
#include <stdio.h>

static analyzer_callbacks_t analyzer_callbacks = {0};
static analyser_config_t analyser_config;

void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks) {
	rpl_event_set_callbacks(callbacks);
}

void rpl_tool_init() {
	desc_poll_init();
	rpldata_init();
	sniffer_parser_init();
	analyser_config.root_rank = 256;
	analyser_config.context0.s6_addr16[0] = 0xaaaa;
    analyser_config.context0.s6_addr16[1] = 0;
    analyser_config.context0.s6_addr16[2] = 0;
    analyser_config.context0.s6_addr16[3] = 0;
    analyser_config.context0.s6_addr16[4] = 0;
    analyser_config.context0.s6_addr16[5] = 0;
    analyser_config.context0.s6_addr16[6] = 0;
    analyser_config.context0.s6_addr16[7] = 0;
}

void rpl_tool_cleanup() {
	desc_poll_cleanup();
}

void rpl_tool_set_analyser_config(const analyser_config_t *config) {
    if ( !config ) return;
    analyser_config = *config;
}

const analyser_config_t *rpl_tool_get_analyser_config() {
    return &analyser_config;
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
