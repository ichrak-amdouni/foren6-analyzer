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
 *         Parent-Child Relationships in DODAG Management
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef LINK_H
#define	LINK_H

#include <stdbool.h>
#include <stdint.h>
#include "metric.h"
#include "address.h"
#include "node.h"

#include "link_type.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_link_ref {
	di_node_ref_t child;
	di_node_ref_t parent;
} di_link_ref_t;

typedef struct di_link_key {
	di_link_ref_t ref;
} di_link_key_t;

size_t link_sizeof();

void link_init(void *data, const void *key, size_t key_size);
void link_destroy(void *data);

void link_key_init(di_link_key_t *key, di_node_ref_t child, di_node_ref_t parent, uint32_t version);
void link_ref_init(di_link_ref_t *ref, di_node_ref_t child, di_node_ref_t parent);
di_link_t *link_dup(const di_link_t *link);

void link_set_key(di_link_t *link, di_link_key_t *key);
void link_set_metric(di_link_t *link, const di_metric_t *metric);
void link_set_user_data(di_link_t *link, void *user_data);
bool link_update(di_link_t *link, time_t time, uint32_t added_packet_count);

bool link_has_changed(di_link_t *link);
void link_reset_changed(di_link_t *link);

const di_link_key_t *link_get_key(const di_link_t *link);
time_t link_get_last_update(const di_link_t *link);
uint32_t link_get_packet_count(const di_link_t *link);
bool link_get_deprecated(const di_link_t *link);
const di_metric_t* link_get_metric(const di_link_t *link);
void *link_get_user_data(const di_link_t *link);

void links_deprecate_all_from(di_link_ref_t const *new_link_ref);
void links_clear_all_deprecated();

#ifdef	__cplusplus
}
#endif

#endif	/* LINK_H */

