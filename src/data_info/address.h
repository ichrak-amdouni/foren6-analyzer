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
 *         Address-related Utility Functions
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef ADDRESS_H
#define	ADDRESS_H

#include <arpa/inet.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define ADDR_MAC64_BROADCAST 0xFFFF

    typedef struct in6_addr addr_ipv6_t;
    typedef uint64_t addr_wpan_t;

#ifdef __APPLE__
#define __in6_u __u6_addr
#ifndef s6_addr16
#define s6_addr16   __u6_addr.__u6_addr16
#endif
#endif

typedef struct di_prefix {
    uint8_t length;
    addr_ipv6_t prefix;
} di_prefix_t;

typedef struct wpan_addr_list {
    addr_wpan_t address;
    struct wpan_addr_list *next;
} wpan_addr_elt_t, *wpan_addr_list_t;

void init_ipv6_addr(addr_ipv6_t * addr);
void init_prefix(di_prefix_t * prefix);

wpan_addr_elt_t *addr_wpan_add_to_list(wpan_addr_list_t * list,
                                       addr_wpan_t address);
wpan_addr_elt_t *addr_wpan_del_from_list(wpan_addr_list_t * list,
                                         addr_wpan_t address);

addr_wpan_t addr_get_mac64_from_ip(addr_ipv6_t ip_address);
addr_ipv6_t addr_get_local_ip_from_mac64(addr_wpan_t mac_address);
addr_ipv6_t addr_get_global_ip_from_mac64(di_prefix_t prefix,
                                          addr_wpan_t mac_address);
uint64_t addr_get_int_id_from_mac64(addr_wpan_t mac_address);
addr_wpan_t addr_get_mac64_from_int_id(uint64_t int_id);

//For list search
//Return 0 if equal
int addr_compare_ip(const addr_ipv6_t * a, const addr_ipv6_t * b);
int addr_compare_wpan(const addr_wpan_t * a, const addr_wpan_t * b);
int addr_compare_ip_len(const addr_ipv6_t * a, const addr_ipv6_t * b, int bit_len); //Compare len bits
int addr_prefix_compare(const di_prefix_t * a, const addr_ipv6_t * b);

int prefix_compare(const di_prefix_t * a, const di_prefix_t * b);

bool addr_is_ip_any(addr_ipv6_t address);
bool addr_is_ip_multicast(addr_ipv6_t address);
bool addr_is_ip_local(addr_ipv6_t address);
bool addr_is_ip_global(addr_ipv6_t address);
bool addr_is_mac64_broadcast(addr_wpan_t address);

#ifdef	__cplusplus
}
#endif
#endif                          /* ADDRESS_H */
