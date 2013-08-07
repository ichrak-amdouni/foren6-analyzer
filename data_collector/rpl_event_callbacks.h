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
} rpl_event_callbacks_t;


void rpl_event_set_callbacks(rpl_event_callbacks_t *callbacks);

void rpl_event_node(di_node_t *node, rpl_event_type_e type);
void rpl_event_dodag(di_dodag_t *dodag, rpl_event_type_e type);
void rpl_event_link(di_link_t *link, rpl_event_type_e type);
void rpl_event_rpl_instance(di_rpl_instance_t *rpl_instance, rpl_event_type_e type);

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_EVENT_CALLBACKS_H */

