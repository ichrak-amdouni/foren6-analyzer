#include <stdio.h>

#include "rpl_event_callbacks.h"

static rpl_event_callbacks_t event_callbacks = {0};

void rpl_event_set_callbacks(rpl_event_callbacks_t *callbacks) {
	event_callbacks = *callbacks;
}

void rpl_event_node(di_node_t *node, rpl_event_type_e type) {
	if(event_callbacks.onNodeEvent) event_callbacks.onNodeEvent(node, type);
}

void rpl_event_dodag(di_dodag_t *dodag, rpl_event_type_e type) {
	if(event_callbacks.onDodagEvent) event_callbacks.onDodagEvent(dodag, type);
}

void rpl_event_link(di_link_t *link, rpl_event_type_e type) {
	if(event_callbacks.onLinkEvent) event_callbacks.onLinkEvent(link, type);
}

void rpl_event_rpl_instance(di_rpl_instance_t *rpl_instance, rpl_event_type_e type) {
	if(event_callbacks.onRplInstanceEvent) event_callbacks.onRplInstanceEvent(rpl_instance, type);
}
