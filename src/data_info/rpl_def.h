/*
 * File:   sniffer_packet_parser.h
 * Author: am
 *
 * Created on June 12, 2013, 4:51 PM
 */

#ifndef RPL_DEF_H
#define RPL_DEF_H

#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "address.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum tag_di_rpl_mop_e {
    RDMOP_NoDownwardRoute,
    RDMOP_NonStoring,
    RDMOP_StoringWithoutMulticast,
    RDMOP_StoringWithMulticast
} di_rpl_mop_e;

typedef enum rpl_dio_opt_metricype_e {
    RDOMT_ETX = 7
} rpl_dio_opt_metric_type_e;

typedef struct rpl_metric {
    rpl_dio_opt_metric_type_e type;
    uint32_t value;
} rpl_metric_t;

typedef struct rpl_prefix {
    di_prefix_t prefix;
    bool on_link;
    bool auto_address_config;
    bool router_address;
    uint32_t valid_lifetime;
    uint32_t preferred_lifetime;
} rpl_prefix_t;

typedef struct rpl_route {
    uint8_t preference;
    uint32_t lifetime;
    uint8_t prefix_bit_length;
    struct in6_addr route_prefix;
} rpl_route_t;

typedef enum tag_di_objective_function_e {
    ROF_ETX = 1
} di_objective_function_e;

typedef struct rpl_dodag_config {
    uint16_t auth_enabled;
    uint8_t path_control_size;
    uint8_t dio_interval_min;
    uint8_t dio_interval_max;
    uint8_t dio_redundancy_constant;
    uint16_t max_rank_inc;
    uint16_t min_hop_rank_inc;
    uint8_t default_lifetime;
    uint16_t lifetime_unit;
    di_objective_function_e objective_function;
} rpl_dodag_config_t;


//DIO
typedef struct rpl_instance_config {
    uint8_t rpl_instance_id;
    uint8_t version_number;
    uint16_t rank;
    bool grounded;
    di_rpl_mop_e mode_of_operation;
    uint8_t dtsn;
    struct in6_addr dodagid;
} rpl_instance_config_t;

//RPL instance config is the same as DIO
typedef struct rpl_instance_config rpl_dio_t;

//Metric configuration is the same as DIO metric option
typedef struct rpl_metric rpl_dio_opt_metric_t;

//Route information is the same as DIO route option
typedef struct rpl_route rpl_dio_opt_route_t;

//DODAG config is the same as DIO config option
typedef struct rpl_dodag_config rpl_dio_opt_config_t;

//Prefix information is the same as DIO prefix  option
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


// DELTA

typedef struct rpl_instance_config_delta {
    bool has_changed;
    bool rpl_instance_id;
    bool version_number;
    bool rank;
    bool grounded;
    bool mode_of_operation;
    bool dtsn;
    bool dodagid;
} rpl_instance_config_delta_t;

typedef struct rpl_dodag_config_delta {
    bool has_changed;
    bool auth_enabled;
    bool path_control_size;
    bool dio_interval_min;
    bool dio_interval_max;
    bool dio_redundancy_constant;
    bool max_rank_inc;
    bool min_hop_rank_inc;
    bool default_lifetime;
    bool lifetime_unit;
    bool objective_function;
} rpl_dodag_config_delta_t;

typedef struct rpl_prefix_delta {
    bool has_changed;
    bool prefix;
    bool on_link;
    bool auto_address_config;
    bool router_address;
    bool valid_lifetime;
    bool preferred_lifetime;
} rpl_prefix_delta_t;

void rpl_instance_config_compare(const rpl_instance_config_t *left, const rpl_instance_config_t *right, rpl_instance_config_delta_t *delta);
void rpl_dodag_config_compare(const rpl_dodag_config_t *left, const rpl_dodag_config_t *right, rpl_dodag_config_delta_t *delta);
void rpl_prefix_compare(const rpl_prefix_t *left, const rpl_prefix_t *right, rpl_prefix_delta_t *delta);

#ifdef  __cplusplus
}
#endif

#endif  /* RPL_DEF_H */

