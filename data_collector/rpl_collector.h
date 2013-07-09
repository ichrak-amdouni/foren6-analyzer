/* 
 * File:   rpl_collector.h
 * Author: am
 *
 * Created on June 21, 2013, 4:47 PM
 */

#ifndef RPL_COLLECTOR_H
#define	RPL_COLLECTOR_H

#include "../sniffer_packet_parser.h"

void rpl_collector_parse_dio(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dio_t* dio, rpl_dio_opt_config_t* dodag_config, rpl_dio_opt_metric_t* metric, rpl_dio_opt_prefix_t* prefix, rpl_dio_opt_route_t* route_info);
void rpl_collector_parse_dao(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dao_t* dao, rpl_dao_opt_target_t* target, rpl_dao_opt_transit_t *transit);
void rpl_collector_parse_dis(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dis_opt_info_req_t *request);
void rpl_collector_parse_data(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_hop_by_hop_opt_t* rpl_info, int packet_id);


#endif	/* RPL_COLLECTOR_H */

