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

#include "data_info/rpl_instance.h"
#include "data_info/dodag.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum rpl_dio_opt_metricype_e {
	RDOMT_ETX = 7
} rpl_dio_opt_metric_type_e;

typedef struct rpl_metric {
	rpl_dio_opt_metric_type_e type;
	uint32_t value;
} rpl_metric_t;

typedef struct rpl_prefix {
	bool on_link;
	bool auto_address_config;
	bool router_address;
	uint32_t valid_lifetime;
	uint32_t preferred_lifetime;
	uint8_t prefix_bit_length;
	struct in6_addr prefix;
} rpl_prefix_t;

typedef struct rpl_route {
	uint8_t preference;
	uint32_t lifetime;
	uint8_t prefix_bit_length;
	struct in6_addr route_prefix;
} rpl_route_t;

//DIO
typedef struct rpl_dio {
	uint8_t rpl_instance_id;
	uint8_t	version_number;
	uint16_t rank;
	bool grounded;
	di_rpl_mop_e mode_of_operation;
	uint8_t dstn;
	struct in6_addr dodagid;
} rpl_dio_t;

typedef struct rpl_metric rpl_dio_opt_metric_t;

typedef struct rpl_route rpl_dio_opt_route_t;

typedef di_dodag_config_t rpl_dio_opt_config_t;

typedef struct rpl_prefix rpl_dio_opt_prefix_t;

//DAO
typedef struct rpl_dao {
	uint8_t rpl_instance_id;
	bool want_ack;
	bool dodagid_present;
	struct in6_addr dodagid;
	uint8_t dao_sequence;
} rpl_dao_t;

typedef struct rpl_dao_opt_target {
	uint8_t target_bit_length;
	struct in6_addr target;
} rpl_dao_opt_target_t;

typedef struct rpl_dao_opt_transit {
	bool external;
	bool parent_addr_present;
	uint8_t path_control;
	uint8_t path_sequence;
	uint8_t path_lifetime;
	struct in6_addr parent_address;
	//Incomplete
} rpl_dao_opt_transit_t;

//DAO-ACK
//Rare ?
typedef enum tag_rpl_dao_ack_status_e {
	DAO_ACK_STATUS_Ok,
	DAO_ACK_STATUS_ShouldNotBeParent,
	DAO_ACK_STATUS_CantBeParent
} rpl_dao_ack_status_e;

typedef struct rpl_dao_ack {
	uint8_t rpl_instance_id;
	bool dodagid_present;
	struct in6_addr dodagid;
	uint8_t dao_sequence;
	rpl_dao_ack_status_e status;
} rpl_dao_ack_t;

typedef struct rpl_dis_opt_info_req {
	bool instance_predicate;
	uint8_t rpl_instance_id;
	bool version_predicate;
	uint8_t rpl_version;
	bool dodag_predicate;
	struct in6_addr dodagid;
} rpl_dis_opt_info_req_t;

//In data packets
typedef struct rpl_hop_by_hop_opt {
	bool packet_toward_root;
	bool rank_error;
	bool forwarding_error;
	uint8_t rpl_instance_id;
	uint16_t sender_rank;
} rpl_hop_by_hop_opt_t;

//Initialize sniffer parser
void sniffer_parser_init();

//Give data to parse to parser
//Call sensor_info_collector_parse_packet when a packet has been fully received
void sniffer_parser_parse_data(const unsigned char* data, int len);


#ifdef	__cplusplus
}
#endif

#endif	/* SNIFFER_PACKET_PARSER_H */

