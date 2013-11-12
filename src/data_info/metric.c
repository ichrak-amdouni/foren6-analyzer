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

#include "metric.h"
#include "../utlist.h"
#include <stdlib.h>
#include <string.h>

static di_metric_type_list_t metrics = NULL;

void
metric_add_type(const di_metric_type_t * metric_model)
{
    di_metric_type_el_t *metric_type;

    metric_type = calloc(1, sizeof(di_metric_type_el_t));
    metric_type->type = calloc(1, sizeof(di_metric_type_t));
    memcpy(metric_type->type, metric_model, sizeof(di_metric_type_t));
    LL_PREPEND(metrics, metric_type);
}

di_metric_type_t *
metric_get_type(const char *name)
{
    di_metric_type_el_t *metric_type;

    LL_FOREACH(metrics, metric_type) {
        if(!strcmp(metric_type->type->name, name))
            return metric_type->type;
    }

    return NULL;
}

char *
metric_to_string(const di_metric_t * metric_value)
{
    if(metric_value->type && metric_value->type->to_string)
        return metric_value->type->to_string(metric_value->value);
    else
        return NULL;
}


double
metric_get_display_value(const di_metric_t * metric_value)
{
    if(metric_value->type && metric_value->type->to_display_value)
        return metric_value->type->to_display_value(metric_value->value);
    else
        return metric_value->value;
}

void
metric_enumerate(metric_enum_callback_t callback)
{
    di_metric_type_el_t *metric_type;

    LL_FOREACH(metrics, metric_type) {
        callback(metric_type->type);
    }
}
