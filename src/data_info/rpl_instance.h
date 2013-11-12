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
 *         RPL Instances
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */


#ifndef RPL_INSTANCE_H
#define	RPL_INSTANCE_H

#include <stdbool.h>
#include <stdint.h>
#include "hash_container.h"

#include "rpl_instance_type.h"
#include "dodag_type.h"
#include "rpl_def.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    int16_t rpl_instance;   //Via DIO, DAO
} di_rpl_instance_ref_t;

typedef struct di_rpl_instance_key {
    di_rpl_instance_ref_t ref;
} di_rpl_instance_key_t;

size_t rpl_instance_sizeof();

void rpl_instance_init(void *data, const void *key, size_t key_size);
void rpl_instance_destroy(void *data);
di_rpl_instance_t *rpl_instance_dup(const di_rpl_instance_t *
                                    rpl_instance);

void rpl_instance_key_init(di_rpl_instance_key_t * key,
                           uint8_t rpl_instance, uint32_t version);
void rpl_instance_ref_init(di_rpl_instance_ref_t * ref,
                           uint8_t rpl_instance);
void rpl_instance_set_key(di_rpl_instance_t * rpl_instance,
                          const di_rpl_instance_key_t * key);
void rpl_instance_set_mop(di_rpl_instance_t * rpl_instance,
                          di_rpl_mop_e mop);
void rpl_instance_set_user_data(di_rpl_instance_t * rpl_instance,
                                void *user_data);
void rpl_instance_add_dodag(di_rpl_instance_t * rpl_instance,
                            di_dodag_t * dodag);
void rpl_instance_del_dodag(di_rpl_instance_t * rpl_instance,
                            di_dodag_t * dodag);

bool rpl_instance_has_changed(di_rpl_instance_t * rpl_instance);
void rpl_instance_reset_changed(di_rpl_instance_t * rpl_instance);

const di_rpl_instance_key_t *rpl_instance_get_key(const di_rpl_instance_t
                                                  * rpl_instance);
di_rpl_mop_e rpl_instance_get_mop(const di_rpl_instance_t * rpl_instance);
void *rpl_instance_get_user_data(const di_rpl_instance_t * rpl_instance);

#ifdef	__cplusplus
}
#endif
#endif                          /* RPL_INSTANCE_H */
