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
 *         6LoWPAN Information
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */


#ifndef SIXLOWPAN_DEF_H_
#define SIXLOWPAN_DEF_H_

#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "address.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct sixlowpan_config {
    bool has_seen_local_address;
    bool is_custom_local_address;
    addr_ipv6_t local_address;

    bool has_seen_global_address;
    bool is_custom_global_address;
    addr_ipv6_t global_address;

    bool is_prefix_valid;
} sixlowpan_config_t;

typedef struct sixlowpan_config_delta {
    bool has_changed;

    bool local_address;
    bool global_address;
} sixlowpan_config_delta_t;

typedef struct sixlowpan_statistics {
    int packet_count;
} sixlowpan_statistics_t;

typedef struct sixlowpan_statistics_delta {
    bool has_changed;
    int packet_count;
} sixlowpan_statistics_delta_t;

typedef struct sixlowpan_errors {
    int invalid_ip;
    int invalid_prefix;
} sixlowpan_errors_t;

typedef struct sixlowpan_errors_delta {
    bool has_changed;
    int invalid_ip;
    int invalid_prefix;
} sixlowpan_errors_delta_t;

void init_sixlowpan_config(sixlowpan_config_t * config);
void init_sixlowpan_statistics(sixlowpan_statistics_t * statistics);
void init_sixlowpan_statistics_delta(sixlowpan_statistics_delta_t *
                                     delta);
void init_sixlowpan_errors(sixlowpan_errors_t * errors);
void init_sixlowpan_errors_delta(sixlowpan_errors_delta_t * delta);

void sixlowpan_config_delta(const sixlowpan_config_t * left,
                            const sixlowpan_config_t * right,
                            sixlowpan_config_delta_t * delta);
void sixlowpan_statistics_delta(const sixlowpan_statistics_t * left,
                                const sixlowpan_statistics_t * right,
                                sixlowpan_statistics_delta_t * delta);
void sixlowpan_errors_delta(const sixlowpan_errors_t * left,
                            const sixlowpan_errors_t * right,
                            sixlowpan_errors_delta_t * delta);

#ifdef  __cplusplus
}
#endif
#endif
