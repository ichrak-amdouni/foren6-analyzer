/* 
 * File:   rpl_collector.h
 * Author: am
 *
 * Created on June 21, 2013, 4:47 PM
 */

#ifndef RPL_COLLECTOR_H
#define	RPL_COLLECTOR_H

#include "../data_info/node.h"
#include "../data_info/dodag.h"
#include "../data_info/rpl_instance.h"

typedef struct rpl_collector {
	di_link_hash_t links;
	di_node_hash_t nodes;
	di_dodag_hash_t dodags;
	di_rpl_instance_hash_t rpl_instances;
} rpl_collector_t;

void rpl_collector_init();
void rpl_collector_dump();
rpl_collector_t *rpl_collector_get();

void rpl_collector_parse_dio(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dio_t* dio, rpl_dio_opt_config_t* dodag_config, rpl_dio_opt_metric_t* metric, rpl_dio_opt_prefix_t* prefix, rpl_dio_opt_route_t* route_info);
void rpl_collector_parse_dao(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dao_t* dao, rpl_dao_opt_target_t* target, rpl_dao_opt_transit_t *transit);
void rpl_collector_parse_dis(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_dis_opt_info_req_t *request);
void rpl_collector_parse_data(uint64_t src_wpan_address, uint64_t dst_wpan_address, struct in6_addr *source_address, struct in6_addr *destination_address, rpl_hop_by_hop_opt_t* rpl_info, int packet_id);


#endif	/* RPL_COLLECTOR_H */

