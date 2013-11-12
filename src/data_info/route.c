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
 *         Routing Table Management
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "route.h"
#include "../utlist.h"

di_route_el_t *
route_get(di_route_list_t * list, di_route_t target, addr_wpan_t via_node,
          bool allow_summary)
{
    di_route_el_t *route;

    LL_FOREACH(*list, route) {
        if(route->via_node == via_node
           && target.length == route->target.length
           && !addr_compare_ip_len(&route->target.prefix, &target.prefix,
                                   target.length))
            return route;
    }

    return NULL;
}

di_route_el_t *
route_add(di_route_list_t * list, di_route_t target, addr_wpan_t via_node,
          bool auto_summary, bool * was_already_existing)
{
    di_route_el_t *route;

    LL_FOREACH(*list, route) {
        if(target.length == route->target.length
           && !addr_compare_ip_len(&route->target.prefix, &target.prefix,
                                   target.length)) {
            if(route->via_node == via_node && was_already_existing)
                *was_already_existing = true;
            else
                route->via_node = via_node;
            return route;
        }
    }

    route = (di_route_el_t *) calloc(1, sizeof(di_route_el_t));
    route->target = target;
    route->via_node = via_node;
    LL_PREPEND(*list, route);

    if(was_already_existing)
        *was_already_existing = false;
    return route;
}

bool
route_remove(di_route_list_t * list, di_route_t target, addr_wpan_t via_node)
{
    di_route_el_t *route;

    LL_FOREACH(*list, route) {
        if(route->via_node == via_node
           && target.length == route->target.length
           && !addr_compare_ip_len(&route->target.prefix, &target.prefix,
                                   target.length)) {
            LL_DELETE(*list, route);
            free(route);
            return true;
        }
    }

    return false;
}

bool
route_del_all_outdated(di_route_list_t * list)
{
    return false;
}

di_route_list_t
route_dup(const di_route_list_t * routes)
{
    di_route_list_t new_routes = NULL;
    di_route_el_t *route, *new_route, *last_route;

    last_route = NULL;
    LL_FOREACH(*routes, route) {
        new_route = (di_route_el_t *) malloc(sizeof(di_route_el_t));
        memcpy(new_route, route, sizeof(di_route_el_t));
        if(last_route)
            last_route->next = new_route;
        new_route->next = NULL;
        if(new_routes == NULL)
            new_routes = new_route;
        last_route = new_route;
    }

    return new_routes;
}

void
route_destroy(di_route_list_t * routes)
{
    di_route_el_t *route, *tmp;

    LL_FOREACH_SAFE(*routes, route, tmp) {
        LL_DELETE(*routes, route);
        free(route);
    }
}
