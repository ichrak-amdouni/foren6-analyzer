/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         RPL Packet Parser
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

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
	analyser_config.root_rank = 256;
	analyser_config.context0.s6_addr16[0] = 0xaaaa;
    analyser_config.context0.s6_addr16[1] = 0;
    analyser_config.context0.s6_addr16[2] = 0;
    analyser_config.context0.s6_addr16[3] = 0;
    analyser_config.context0.s6_addr16[4] = 0;
    analyser_config.context0.s6_addr16[5] = 0;
    analyser_config.context0.s6_addr16[6] = 0;
    analyser_config.context0.s6_addr16[7] = 0;
    analyser_config.address_autconf_only = true;
    analyser_config.one_preferred_parent = true;
}

void rpl_tool_start() {
    sniffer_parser_init();
}

void rpl_tool_cleanup() {
	desc_poll_cleanup();
}

void rpl_tool_start_capture() {
    sniffer_parser_reset();
    sniffer_parser_create_out();
}

void rpl_tool_stop_capture() {
    sniffer_parser_close_out();
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
