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
 *         RPL Data Access
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPL_DATA_H
#define	RPL_DATA_H

#include "link.h"
#include "node.h"
#include "dodag.h"
#include "rpl_instance.h"
#include "hash_container.h"

#ifdef	__cplusplus
extern "C" {
#endif

void rpldata_init();

//These function use version 0. Modification to other version should not be allowed.
di_node_t *rpldata_get_node(const di_node_ref_t *node_ref, hash_value_mode_e value_mode, bool *was_already_existing);
di_dodag_t *rpldata_get_dodag(const di_dodag_ref_t *dodag_ref, hash_value_mode_e value_mode, bool *was_already_existing);
di_rpl_instance_t *rpldata_get_rpl_instance(const di_rpl_instance_ref_t *rpl_instance_ref, hash_value_mode_e value_mode, bool *was_already_existing);
di_link_t *rpldata_get_link(const di_link_ref_t *link_ref, hash_value_mode_e value_mode, bool *was_already_existing);
di_link_t *rpldata_del_link(const di_link_ref_t *link_ref);

hash_container_ptr rpldata_get_nodes(uint32_t version);
hash_container_ptr rpldata_get_dodags(uint32_t version);
hash_container_ptr rpldata_get_rpl_instances(uint32_t version);
hash_container_ptr rpldata_get_links(uint32_t version);


uint32_t rpldata_add_node_version();
uint32_t rpldata_add_dodag_version();
uint32_t rpldata_add_rpl_instance_version();
uint32_t rpldata_add_link_version();

void rpldata_wsn_create_version(int packed_id, double timestamp);
double rpldata_wsn_version_get_timestamp(uint32_t version);
uint32_t rpldata_wsn_version_get_packet_count(uint32_t version);
uint32_t rpldata_wsn_version_get_has_errors(uint32_t version);

uint32_t rpldata_get_wsn_last_version();

void rpldata_clear();

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_DATA_H */

