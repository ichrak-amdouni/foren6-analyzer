#include <stdio.h>

#include "rpl_event_callbacks.h"

static rpl_event_callbacks_t event_callbacks = {0};

void rpl_event_set_callbacks(rpl_event_callbacks_t *callbacks) {
	event_callbacks = *callbacks;
}

void rpl_event_node_created(di_node_t *node) {
	if(event_callbacks.onNodeCreated) event_callbacks.onNodeCreated(node);
}

void rpl_event_node_deleted(di_node_t *node) {
	if(event_callbacks.onNodeDeleted) event_callbacks.onNodeDeleted(node);
}

void rpl_event_node_updated(di_node_t *node) {
	if(event_callbacks.onNodeUpdate) event_callbacks.onNodeUpdate(node);
}


void rpl_event_dodag_created(di_dodag_t *dodag) {
	if(event_callbacks.onDodagCreated) event_callbacks.onDodagCreated(dodag);
}

void rpl_event_dodag_deleted(di_dodag_t *dodag) {
	if(event_callbacks.onDodagDeleted) event_callbacks.onDodagDeleted(dodag);
}

void rpl_event_dodag_updated(di_dodag_t *dodag) {
	if(event_callbacks.onDodagUpdated) event_callbacks.onDodagUpdated(dodag);
}

void rpl_event_link_created(di_link_t *link) {
	if(event_callbacks.onLinkCreated) event_callbacks.onLinkCreated(link);
}

void rpl_event_link_deleted(di_link_t *link) {
	if(event_callbacks.onLinkDeleted) event_callbacks.onLinkDeleted(link);
}

void rpl_event_link_updated(di_link_t *link) {
	if(event_callbacks.onLinkUpdated) event_callbacks.onLinkUpdated(link);
}


void rpl_event_rpl_instance_created(di_rpl_instance_t *rpl_instance) {
	if(event_callbacks.onRplInstanceCreated) event_callbacks.onRplInstanceCreated(rpl_instance);
}

void rpl_event_rpl_instance_deleted(di_rpl_instance_t *rpl_instance) {
	if(event_callbacks.onRplInstanceDeleted) event_callbacks.onRplInstanceDeleted(rpl_instance);
}

void rpl_event_rpl_instance_updated(di_rpl_instance_t *rpl_instance) {
	if(event_callbacks.onRplInstanceUpdated) event_callbacks.onRplInstanceUpdated(rpl_instance);
}
