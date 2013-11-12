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
 *         Sniffer Packet Parser
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */


#ifndef SNIFFER_PACKET_PARSER_H
#define	SNIFFER_PACKET_PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "data_info/rpl_instance.h"
#include "data_info/dodag.h"
#include "data_info/rpl_def.h"

#ifdef	__cplusplus
extern "C" {
#endif

//Initialize sniffer parser
void sniffer_parser_init();
void sniffer_parser_reset();

//Give data to parse to parser
//Call sensor_info_collector_parse_packet when a packet has been fully received
void sniffer_parser_parse_data(const unsigned char* data, int len, struct timeval timestamp);

int sniffer_parser_get_packet_count();
void sniffer_parser_pause_parser(bool pause);

void sniffer_parser_create_out();
void sniffer_parser_close_out();

#ifdef	__cplusplus
}
#endif

#endif	/* SNIFFER_PACKET_PARSER_H */

