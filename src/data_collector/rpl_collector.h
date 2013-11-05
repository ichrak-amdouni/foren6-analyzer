/*
 * File:   rpl_collector.h
 * Author: am
 *
 * Created on June 21, 2013, 4:47 PM
 */

#ifndef RPL_COLLECTOR_H
#define	RPL_COLLECTOR_H

#include "../sniffer_packet_parser.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct packet_info {
	addr_wpan_t src_wpan_address;
	addr_wpan_t dst_wpan_address;
	addr_ipv6_t src_ip_address;
	addr_ipv6_t dst_ip_address;
	double timestamp;
	int hop_limit;
} packet_info_t;

void rpl_collector_parse_dio(packet_info_t pkt_info, rpl_dio_t* dio, rpl_dio_opt_config_t* dodag_config, rpl_dio_opt_metric_t* metric, rpl_dio_opt_prefix_t* prefix, rpl_dio_opt_route_t* route_info);
void rpl_collector_parse_dao(packet_info_t pkt_info, rpl_dao_t* dao, rpl_dao_opt_target_t* target, rpl_dao_opt_transit_t *transit);
void rpl_collector_parse_dis(packet_info_t pkt_info, rpl_dis_opt_info_req_t *request);
void rpl_collector_parse_data(packet_info_t pkt_info, rpl_hop_by_hop_opt_t* rpl_info);

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_COLLECTOR_H */

