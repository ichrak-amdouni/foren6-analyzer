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

#ifndef RPL_PACKET_PARSER_H
#define	RPL_PACKET_PARSER_H

#include "data_info/rpl_data.h"
#include "data_collector/rpl_event_callbacks.h"
#include "interface_reader/interfaces_mgr.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct analyzer_callbacks {
    void (*onErrorEvent)(char const * errorMessage);
} analyzer_callbacks_t;

typedef struct analyser_config {
    int root_rank;
    struct in6_addr context0;
    bool address_autconf_only;
    bool one_preferred_parent;
} analyser_config_t;

	/**
	 * Initialize the analyzer.
     */
	void rpl_tool_init();

    void rpl_tool_start();

	void rpl_tool_cleanup();

	void rpl_tool_start_capture();

	void rpl_tool_stop_capture();

	/**
	 * Configure the analysis parameter
	 */
	void rpl_tool_set_analyser_config(const analyser_config_t *config);
	const analyser_config_t *rpl_tool_get_analyser_config();

	interface_t *rpl_tool_get_interface(const char *name);

	/**
	 * Set callback to call when events are triggered (like node creation)
     * @param callbacks
     */
	void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks);

    /**
     * Set callback to call when analyzer events are triggered (like error reporting)
     * @param callbacks
     */
	void rpl_tool_set_analyzer_callbacks(analyzer_callbacks_t * callbacks);

	void rpl_tool_report_error(char const *  error_message);

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_PACKET_PARSER_H */

