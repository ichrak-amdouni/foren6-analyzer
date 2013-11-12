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
 *         RPL Metrics
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef METRIC_H
#define	METRIC_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_metric_type {
    const char *name;
    char *(*to_string) (uint64_t value);
    double (*to_display_value) (uint64_t value);
} di_metric_type_t;

typedef struct di_metric_type_el {
    di_metric_type_t *type;
    struct di_metric_type_el *next;
} di_metric_type_el_t, *di_metric_type_list_t;

typedef struct di_metric {
    di_metric_type_t *type;
    uint64_t value;
} di_metric_t;

typedef void (*metric_enum_callback_t) (di_metric_type_t * metric_type);

void metric_add_type(const di_metric_type_t * metric_model);
di_metric_type_t *metric_get_type(const char *name);
char *metric_to_string(const di_metric_t * metric_value);
double metric_get_display_value(const di_metric_t * metric_value);
void metric_enumerate(metric_enum_callback_t callback);

#ifdef	__cplusplus
}
#endif
#endif                          /* METRIC_H */
