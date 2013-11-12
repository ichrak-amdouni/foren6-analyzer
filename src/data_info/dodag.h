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
 *         RPL DODAG Information Mangement
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
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
    addr_ipv6_t dodagid;    //Via DIO, DAO
    int16_t version;        //Via DIO
} di_dodag_ref_t;

typedef struct di_dodag_key {
    di_dodag_ref_t ref;
} di_dodag_key_t;

size_t dodag_sizeof();

void dodag_init(void *data, const void *key, size_t key_size);
void dodag_destroy(void *data);
di_dodag_t *dodag_dup(const di_dodag_t * dodag);

void dodag_key_init(di_dodag_key_t * key, addr_ipv6_t dodag_id,
                    uint8_t dodag_version, uint32_t version);
void dodag_ref_init(di_dodag_ref_t * ref, addr_ipv6_t dodag_id,
                    uint8_t dodag_version);
void dodag_set_key(di_dodag_t * dodag, const di_dodag_key_t * key);
const di_dodag_key_t *dodag_get_key(const di_dodag_t * dodag);
bool dodag_has_changed(di_dodag_t * dodag);
void dodag_reset_changed(di_dodag_t * dodag);
void dodag_set_nodes_changed(di_dodag_t * dodag);
void dodag_set_user_data(di_dodag_t * dodag, void *user_data);
void *dodag_get_user_data(const di_dodag_t * dodag);

void dodag_update_from_dio(di_dodag_t * dodag, const rpl_dio_t * dio);
void dodag_update_from_dodag_config(di_dodag_t * dodag,
                                    const rpl_dodag_config_t *
                                    dodag_config);
void dodag_update_from_dodag_prefix_info(di_dodag_t * dodag,
                                         const rpl_prefix_t * prefix);
void dodag_set_rpl_instance(di_dodag_t * dodag,
                            const di_rpl_instance_ref_t * rpl_instance);

void dodag_add_node(di_dodag_t * dodag, di_node_t * node);
void dodag_del_node(di_dodag_t * dodag, di_node_t * node);

const rpl_instance_config_t *dodag_get_instance_config(const di_dodag_t *
                                                       dodag);
const rpl_dodag_config_t *dodag_get_dodag_config(const di_dodag_t *
                                                 dodag);
const rpl_prefix_t *dodag_get_prefix(const di_dodag_t * dodag);
const di_rpl_instance_ref_t *dodag_get_rpl_instance(const di_dodag_t *
                                                    dodag);
hash_container_ptr dodag_get_node(const di_dodag_t * dodag);

#ifdef	__cplusplus
}
#endif
#endif                          /* DODAG_H */
