/*
 * File:   dodag.h
 * Author: am
 *
 * Created on June 20, 2013, 5:04 PM
 */

#ifndef DODAG_H
#define	DODAG_H

#include "address.h"
#include "rpl_instance.h"
#include "hash_container.h"

#include "node_type.h"
#include "dodag_type.h"
#include "rpl_def.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_dodag_ref {
	addr_ipv6_t dodagid;				//Via DIO, DAO
	int16_t version;					//Via DIO
} di_dodag_ref_t;

typedef struct di_dodag_key {
	di_dodag_ref_t ref;
} di_dodag_key_t;

size_t dodag_sizeof();

void dodag_init(void *data, const void *key, size_t key_size);
void dodag_destroy(void *data);
di_dodag_t *dodag_dup(const di_dodag_t *dodag);

void dodag_key_init(di_dodag_key_t *key, addr_ipv6_t dodag_id, uint8_t dodag_version, uint32_t version);
void dodag_ref_init(di_dodag_ref_t *ref, addr_ipv6_t dodag_id, uint8_t dodag_version);
void dodag_set_key(di_dodag_t *dodag, const di_dodag_key_t *key);
void dodag_set_instance_config(di_dodag_t *dodag, const rpl_instance_config_t *instance_config);
void dodag_set_dodag_config(di_dodag_t *dodag, const rpl_dodag_config_t *dodag_config);
void dodag_set_prefix(di_dodag_t *dodag, const rpl_prefix_t *prefix);
void dodag_set_rpl_instance(di_dodag_t *dodag, const di_rpl_instance_ref_t* rpl_instance);
void dodag_add_node(di_dodag_t *dodag, di_node_t *node);
void dodag_del_node(di_dodag_t *dodag, di_node_t *node);
void dodag_set_user_data(di_dodag_t *dodag, void *user_data);

bool dodag_has_changed(di_dodag_t *dodag);
void dodag_reset_changed(di_dodag_t *dodag);

const di_dodag_key_t *dodag_get_key(const di_dodag_t *dodag);
const rpl_instance_config_t *dodag_get_instance_config(const di_dodag_t *dodag);
const rpl_dodag_config_t *dodag_get_dodag_config(const di_dodag_t *dodag);
const rpl_prefix_t *dodag_get_prefix(const di_dodag_t *dodag);
const di_rpl_instance_ref_t *dodag_get_rpl_instance(const di_dodag_t *dodag);
hash_container_ptr dodag_get_node(const di_dodag_t *dodag);
void *dodag_get_user_data(const di_dodag_t *dodag);


#ifdef	__cplusplus
}
#endif


#endif	/* DODAG_H */

