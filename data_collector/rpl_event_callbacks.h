/* 
 * File:   rpl_event_callbacks.h
 * Author: am
 *
 * Created on June 27, 2013, 3:10 PM
 */

#ifndef RPL_EVENT_CALLBACKS_H
#define	RPL_EVENT_CALLBACKS_H

#include "../data_info/node.h"
#include "../data_info/dodag.h"
#include "../data_info/rpl_instance.h"
#include "../data_info/link.h"

typedef struct rpl_event_callbacks {
	void (*onNodeCreated)(di_node_t *node);
	void (*onNodeDeleted)(di_node_t *node);
	void (*onNodeUpdate)(di_node_t *node);
	void (*onDodagCreated)(di_dodag_t *dodag);
	void (*onDodagDeleted)(di_dodag_t *dodag);
	void (*onDodagUpdated)(di_dodag_t *dodag);
	void (*onLinkCreated)(di_link_t *link);
	void (*onLinkDeleted)(di_link_t *link);
	void (*onLinkUpdated)(di_link_t *link);
	void (*onRplInstanceCreated)(di_rpl_instance_t *rpl_instance);
	void (*onRplInstanceUpdated)(di_rpl_instance_t *rpl_instance);
	void (*onRplInstanceDeleted)(di_rpl_instance_t *rpl_instance);
} rpl_event_callbacks_t;


void rpl_event_set_callbacks(rpl_event_callbacks_t *callbacks);
void rpl_event_node_created(di_node_t *node);
void rpl_event_node_deleted(di_node_t *node);
void rpl_event_node_updated(di_node_t *node);

void rpl_event_dodag_created(di_dodag_t *dodag);
void rpl_event_dodag_deleted(di_dodag_t *dodag);
void rpl_event_dodag_updated(di_dodag_t *dodag);

void rpl_event_link_created(di_link_t *link);
void rpl_event_link_deleted(di_link_t *link);
void rpl_event_link_updated(di_link_t *link);

void rpl_event_rpl_instance_created(di_rpl_instance_t *rpl_instance);
void rpl_event_rpl_instance_deleted(di_rpl_instance_t *rpl_instance);
void rpl_event_rpl_instance_updated(di_rpl_instance_t *rpl_instance);

#endif	/* RPL_EVENT_CALLBACKS_H */

