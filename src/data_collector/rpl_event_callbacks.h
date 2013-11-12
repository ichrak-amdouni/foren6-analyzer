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
 *         RPL event callbacks
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPL_EVENT_CALLBACKS_H
#define	RPL_EVENT_CALLBACKS_H

#include "../data_info/node.h"
#include "../data_info/dodag.h"
#include "../data_info/rpl_instance.h"
#include "../data_info/link.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum rpl_event_type {
	RET_Created,
	RET_Updated,
	RET_Deleted
} rpl_event_type_e;

typedef struct rpl_event_callbacks {
	void (*onNodeEvent)(di_node_t *node, rpl_event_type_e event_type);
	void (*onDodagEvent)(di_dodag_t *dodag, rpl_event_type_e event_type);
	void (*onLinkEvent)(di_link_t *link, rpl_event_type_e event_type);
	void (*onRplInstanceEvent)(di_rpl_instance_t *rpl_instance, rpl_event_type_e event_type);
	void (*onPacketEvent)(int packet_id);
	void (*onClearEvent)();
} rpl_event_callbacks_t;


void rpl_event_set_callbacks(rpl_event_callbacks_t *callbacks);

void rpl_event_packet(int packet_id);
void rpl_event_node(di_node_t *node, rpl_event_type_e type);
void rpl_event_dodag(di_dodag_t *dodag, rpl_event_type_e type);
void rpl_event_link(di_link_t *link, rpl_event_type_e type);
void rpl_event_rpl_instance(di_rpl_instance_t *rpl_instance, rpl_event_type_e type);

//create a WSN version if needed and return true if at least one object has changed
bool rpl_event_commit_changed_objects(int packet_id, double timestamp);

void rpl_event_process_events(int wsn_version);

void rpl_event_clear();

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_EVENT_CALLBACKS_H */

