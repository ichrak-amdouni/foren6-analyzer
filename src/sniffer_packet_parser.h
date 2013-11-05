/*
 * File:   sniffer_packet_parser.h
 * Author: am
 *
 * Created on June 12, 2013, 4:51 PM
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

