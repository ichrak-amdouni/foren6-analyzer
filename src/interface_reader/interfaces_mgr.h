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
 *         Interfaces Management
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef INTERFACES_REGISTER_H
#define	INTERFACES_REGISTER_H

#include <stdbool.h>
#include <sys/time.h>
#include "../data_info/hash_container.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct ifinstance {
    void *interface_data;
    const char *target;     //for information only, not used

    //For timestamp sync
    struct timeval delta_to_parent;
    struct ifinstance *parent;
    hash_container_ptr last_packets;
    bool first_packet;
    bool ethernet;
    bool fcs;

    //for lists
    struct ifinstance *next;
} ifinstance_t, *ifreader_t;

typedef struct interface {
    const char *interface_name;
    void (*init) ();
      ifreader_t(*open) (const char *target, int channel);
      bool(*start) (ifreader_t handle);
    void (*stop) (ifreader_t handle);
    void (*close) (ifreader_t handle);

} interface_t;

typedef void (*interface_enum_function_t) (const char *interface_name);
typedef int (*interface_version_function_t) ();
typedef interface_t(*interface_register_function_t) ();

interface_t *interfacemgr_get(const char *name);

ifinstance_t *interfacemgr_create_handle(const char *target);
void interfacemgr_destroy_handle(ifinstance_t * handle);

struct timeval interfacemgr_get_absolute_timestamp(const ifinstance_t *
                                                   iface,
                                                   struct timeval
                                                   packet_timestamp);
void interfacemgr_set_origin(ifinstance_t * iface,
                             struct timeval packet_timestamp);
void inferfacemgr_set_master_time(ifinstance_t * iface,
                                  struct timeval packet_timestamp);
ifinstance_t *interfacemgr_get_root(ifinstance_t * iface);
void interfacemgr_rebase_parent(ifinstance_t * iface);
void interfacemgr_sync_time(ifinstance_t * iface1,
                            struct timeval packet_timestamp1,
                            ifinstance_t * iface2,
                            struct timeval packet_timestamp2);
void interfacemgr_process_packet(ifinstance_t * iface,
                                 const unsigned char *data, int len,
                                 struct timeval timestamp);
bool interfacemgr_check_multisniffer_packet(ifinstance_t * iface,
                                            ifinstance_t * other,
                                            const unsigned char *data,
                                            int len,
                                            struct timeval timestamp);
bool interfacemgr_check_duplicate_packet(ifinstance_t * iface,
                                         const unsigned char *data,
                                         int len,
                                         struct timeval timestamp);
struct packet_data *interfacemgr_get_old_packet(ifinstance_t * iface,
                                                const unsigned char *data,
                                                int len);

#ifdef	__cplusplus
}
#endif
#endif                          /* INTERFACES_REGISTER_H */
