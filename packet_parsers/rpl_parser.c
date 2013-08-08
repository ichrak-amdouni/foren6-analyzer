#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "rpl_parser.h"
#include "parser_register.h"
#include "../data_collector/rpl_collector.h"
#include "../descriptor_poll.h"
#ifdef __APPLE__
#include "../apple-endian.h"
#endif

#include "../data_info/metric.h"

#define ICMPV6_RPL_TYPE 155
#define ICMPV6_RPL_CODE_DIS     0x0
#define ICMPV6_RPL_CODE_DIO     0x1
#define ICMPV6_RPL_CODE_DAO     0x2
#define ICMPV6_RPL_CODE_DAO_ACK 0x3
#define ICMPV6_RPL_CODE_CC      0xA           //Only in secure mode
#define ICMPV6_RPL_CODE_SECURE_MASK 0x80
#define ICMPV6_RPL_CODE_MASK 0x0F

typedef enum rpl_packet_type {
	RPT_None,
	RPT_RplUnknown,
	RPT_DIS,
	RPT_DIO,
	RPT_DAO,
	RPT_Data
} rpl_packet_type_e;

typedef struct rpl_packet_dis {
	bool has_info;
	rpl_dis_opt_info_req_t info;
} rpl_packet_dis_t;

typedef struct rpl_packet_dio {
	rpl_dio_t dio;
	bool has_config;
	bool has_prefix;
	bool has_route;
	bool has_metric;
	rpl_dio_opt_config_t config;
	rpl_dio_opt_prefix_t prefix;
	rpl_dio_opt_route_t  route;
	rpl_dio_opt_metric_t metric;
} rpl_packet_dio_t;

typedef struct rpl_packet_dao {
	rpl_dao_t dao;
	bool has_target;
	bool has_transit;
	rpl_dao_opt_target_t target;
	rpl_dao_opt_transit_t transit;
} rpl_packet_dao_t;

typedef struct rpl_packet_data {
	bool has_hop_info;
	rpl_hop_by_hop_opt_t hop_info;
} rpl_packet_data_t;

typedef struct rpl_packet_content {
	int packet_id;
	rpl_packet_type_e type;
	bool is_bad;
	uint64_t src_wpan_address;
	uint64_t dst_wpan_address;
	struct in6_addr src_ip_address;
	struct in6_addr dst_ip_address;
	struct timeval timestamp;
	union {
		rpl_packet_dis_t dis;
		rpl_packet_dio_t dio;
		rpl_packet_dao_t dao;
		rpl_packet_data_t data;
	};
} rpl_packet_content_t;

static rpl_packet_content_t current_packet;

static double rpl_parser_metric_etx_to_double(uint64_t value);
static char* rpl_parser_metric_etx_to_string(uint64_t value);

static void rpl_parser_begin_packet();
static void rpl_parser_parse_field(const char *nameStr, const char *showStr, const char *valueStr, int64_t valueInt);
static void rpl_parser_end_packet();

parser_t rpl_parser_register() {
	parser_t parser;
	di_metric_type_t metric_etx;

	parser.parser_name = "rpl";
	parser.begin_packet = &rpl_parser_begin_packet;
	parser.parse_field = &rpl_parser_parse_field;
	parser.end_packet = &rpl_parser_end_packet;

	metric_etx.name = "ETX";
	metric_etx.to_display_value = &rpl_parser_metric_etx_to_double;
	metric_etx.to_string = &rpl_parser_metric_etx_to_string;
	metric_add_type(&metric_etx);

	return parser;
}

static double rpl_parser_metric_etx_to_double(uint64_t value) {
	return value / 128.0;
}

static char *rpl_parser_metric_etx_to_string(uint64_t value) {
	char *str = malloc(10);
	sprintf(str, "%f", value/128.0);
	return str;
}

static void rpl_parser_begin_packet() {
		memset(&current_packet, 0, sizeof(current_packet));
		current_packet.type = RPT_None;
}

static void rpl_parser_parse_field(const char *nameStr, const char *showStr, const char *valueStr, int64_t valueInt) {
	//fprintf(stderr, "XML element: name= %s, show= %s, value= %lld(%s)\n", nameStr, showStr, valueInt, valueStr);

	if(current_packet.is_bad)
		return;

	if(valueStr == NULL) {
		if(!strcmp(nameStr, "frame.number")) {
			current_packet.packet_id = strtol(showStr, NULL, 10);
		}
	} else if(!strcmp(nameStr, "icmpv6.checksum_bad") && !strcmp(showStr, "1")) {
		current_packet.is_bad = true;
	} else if(current_packet.type == RPT_None || current_packet.type == RPT_RplUnknown || current_packet.type == RPT_Data) {
		if(!strcmp(nameStr, "frame.time_epoch")) {
			char *endptr = 0;
			current_packet.timestamp.tv_sec = strtol(showStr, &endptr, 10);
			if(endptr != 0 && *endptr == '.')
				current_packet.timestamp.tv_usec = strtod(endptr, NULL)/1000000.0;
		} else if(!strcmp(nameStr, "wpan.src64")) {
			uint64_t addr = strtoull(valueStr, NULL, 16);
			current_packet.src_wpan_address = htobe64(addr);
		} else if(!strcmp(nameStr, "wpan.dst64")) {
			uint64_t addr = strtoull(valueStr, NULL, 16);
			current_packet.dst_wpan_address = htobe64(addr);
		} else if(!strcmp(nameStr, "ipv6.src")) {
			 inet_pton(AF_INET6, showStr, &current_packet.src_ip_address);
			 if(current_packet.type == RPT_None)
				 current_packet.type = RPT_Data;
		} else if(!strcmp(nameStr, "ipv6.dst")) {
			 inet_pton(AF_INET6, showStr, &current_packet.dst_ip_address);
			 if(current_packet.type == RPT_None)
				 current_packet.type = RPT_Data;
		} else if(!strcmp(nameStr, "icmpv6.type") && valueInt == ICMPV6_RPL_TYPE) {
			current_packet.type = RPT_RplUnknown;
		} else if(current_packet.type == RPT_RplUnknown && !strcmp(nameStr, "icmpv6.code")) {
			//We have a RPL packet, check its code
			//We do not support SECURE packet at this time, so we check for non secure packets and don't mask with ICMPV6_RPL_CODE_MASK
			switch(valueInt) {
				case ICMPV6_RPL_CODE_DIS:
					current_packet.type = RPT_DIS;
					break;

				case ICMPV6_RPL_CODE_DIO:
					current_packet.type = RPT_DIO;
					break;

				case ICMPV6_RPL_CODE_DAO:
					current_packet.type = RPT_DAO;
					break;

				default:
					fprintf(stderr, "Unsupported RPL packet code: %lld\n", valueInt);
					break;
			}
		}
	}
	if(current_packet.type != RPT_None || current_packet.type != RPT_RplUnknown) {
		bool option_check;
		switch(current_packet.type) {
			case RPT_DIS:
				if(current_packet.dis.has_info == false && strstr(nameStr, "icmpv6.rpl.opt.solicited"))
					current_packet.dis.has_info = true;

				if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.flag.d"))
					current_packet.dis.info.dodag_predicate = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.flag.i"))
					current_packet.dis.info.instance_predicate = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.flag.v"))
					current_packet.dis.info.version_predicate = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.dodagid"))
					inet_pton(AF_INET6, showStr, &current_packet.dis.info.dodag_predicate);
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.instance"))
					current_packet.dis.info.rpl_instance_id = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.solicited.version"))
					current_packet.dis.info.rpl_version = valueInt;
				break;

			case RPT_DIO:
				if(!strcmp(nameStr, "icmpv6.rpl.dio.instance"))
					current_packet.dio.dio.rpl_instance_id = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.version"))
					current_packet.dio.dio.version_number = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.rank"))
					current_packet.dio.dio.rank = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.flag.g"))
					current_packet.dio.dio.grounded = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.flag.mop"))
					current_packet.dio.dio.mode_of_operation = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.dstn"))
					current_packet.dio.dio.dstn = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dio.dagid"))
					inet_pton(AF_INET6, showStr, &current_packet.dio.dio.dodagid);

				//Metric option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.metric.type"))
					current_packet.dio.metric.type = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.metric.etx"))	//Only ETX supported in wireshark at this time
					current_packet.dio.metric.value = valueInt;
				else option_check = false;
				if(option_check)
					current_packet.dio.has_metric = true;

				//Route option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.route.pref"))
					current_packet.dio.route.preference = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.route.lifetime"))
					current_packet.dio.route.lifetime = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.route.prefix_length"))
					current_packet.dio.route.prefix_bit_length = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.route.prefix"))
					inet_pton(AF_INET6, showStr, &current_packet.dio.route.route_prefix);
				else option_check = false;
				if(option_check)
					current_packet.dio.has_route = true;

				//config option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.config.auth"))
					current_packet.dio.config.auth_enabled = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.pcs"))
					current_packet.dio.config.path_control_size = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.interval_min"))
					current_packet.dio.config.dio_interval_min = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.interval_double"))
					current_packet.dio.config.dio_interval_max = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.redundancy"))
					current_packet.dio.config.dio_redundancy_constant = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.max_rank_inc"))
					current_packet.dio.config.max_rank_inc = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.min_hop_rank_inc"))
					current_packet.dio.config.min_hop_rank_inc = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.def_lifetime"))
					current_packet.dio.config.default_lifetime = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.lifetime_unit"))
					current_packet.dio.config.lifetime_unit = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.ocp"))
					current_packet.dio.config.objective_function = valueInt;
				else option_check = false;
				if(option_check)
					current_packet.dio.has_config = true;

				//Prefix option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.prefix.flag.l"))
					current_packet.dio.prefix.on_link = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.prefix.preferred_lifetime"))
					current_packet.dio.prefix.preferred_lifetime = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.prefix.valid_lifetime"))
					current_packet.dio.prefix.valid_lifetime = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.prefix.length"))
					current_packet.dio.prefix.prefix_bit_length = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.flag.a"))
					current_packet.dio.prefix.auto_address_config = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.config.flag.r"))
					current_packet.dio.prefix.router_address = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.prefix"))
					inet_pton(AF_INET6, showStr, &current_packet.dio.prefix.prefix);
				else option_check = false;
				if(option_check)
					current_packet.dio.has_prefix = true;
				break;

			case RPT_DAO:
				if(!strcmp(nameStr, "icmpv6.rpl.dao.instance"))
					current_packet.dao.dao.rpl_instance_id = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dao.flag.k"))
					current_packet.dao.dao.want_ack = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dao.flag.d"))
					current_packet.dao.dao.dodagid_present = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.dao.dodagid"))
					inet_pton(AF_INET6, showStr, &current_packet.dao.dao.dodagid);
				else if(!strcmp(nameStr, "icmpv6.rpl.dao.sequence"))
					current_packet.dao.dao.dao_sequence = valueInt;

				//Target option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.target.prefix_length"))
					current_packet.dao.target.target_bit_length = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.target.prefix"))
					inet_pton(AF_INET6, showStr, &current_packet.dao.target.target);
				else option_check = false;
				if(option_check)
					current_packet.dao.has_target = true;

				//Transit option
				option_check = true;
				if(!strcmp(nameStr, "icmpv6.rpl.opt.transit.flag.e"))
					current_packet.dao.transit.external = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.transit.pathctl"))
					current_packet.dao.transit.path_control = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.transit.pathseq"))
					current_packet.dao.transit.path_sequence = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.transit.pathlifetime"))
					current_packet.dao.transit.path_lifetime = valueInt;
				else if(!strcmp(nameStr, "icmpv6.rpl.opt.transit.parent")) {
					current_packet.dao.transit.parent_addr_present = true;
					inet_pton(AF_INET6, showStr, &current_packet.dao.transit.parent_address);
				} else option_check = false;
				if(option_check)
					current_packet.dao.has_transit = true;
				break;

			case RPT_Data:
				//Transit option
				option_check = true;
				if(!strcmp(nameStr, "ipv6.opt.rpl.flag.f"))
					current_packet.data.hop_info.forwarding_error = valueInt;
				else if(!strcmp(nameStr, "ipv6.opt.rpl.flag.o"))
					current_packet.data.hop_info.packet_toward_root = !valueInt;
				else if(!strcmp(nameStr, "ipv6.opt.rpl.flag.r"))
					current_packet.data.hop_info.rank_error = valueInt;
				else if(!strcmp(nameStr, "ipv6.opt.rpl.instance_id"))
					current_packet.data.hop_info.rpl_instance_id = valueInt;
				else if(!strcmp(nameStr, "ipv6.opt.rpl.sender_rank")) {
					current_packet.data.hop_info.sender_rank = valueInt;
				} else option_check = false;
				if(option_check)
					current_packet.data.has_hop_info = true;
				break;

			default:
				break;
		}
	}
}

static void rpl_parser_end_packet() {
	/* Packet end, check if it's a valid RPL packet and call the appropriate function */
	if(current_packet.is_bad) {
		//fprintf(stderr, "Bad packet %d\n", current_packet.type);
		return;
	}

	if(current_packet.type != RPT_None && current_packet.type != RPT_RplUnknown) {
		switch(current_packet.type) {
			case RPT_DIS:
				rpl_collector_parse_dis(
						current_packet.src_wpan_address,
						current_packet.dst_wpan_address,
						&current_packet.src_ip_address,
						&current_packet.dst_ip_address,
						(current_packet.dis.has_info)? &current_packet.dis.info : NULL);
				break;

			case RPT_DIO:
				rpl_collector_parse_dio(
						current_packet.src_wpan_address,
						current_packet.dst_wpan_address,
						&current_packet.src_ip_address,
						&current_packet.dst_ip_address,
						&current_packet.dio.dio,
						(current_packet.dio.has_config)? &current_packet.dio.config : NULL,
						(current_packet.dio.has_metric)? &current_packet.dio.metric : NULL,
						(current_packet.dio.has_prefix)? &current_packet.dio.prefix : NULL,
						(current_packet.dio.has_route)?  &current_packet.dio.route  : NULL);
				break;

			case RPT_DAO:
				rpl_collector_parse_dao(
						current_packet.src_wpan_address,
						current_packet.dst_wpan_address,
						&current_packet.src_ip_address,
						&current_packet.dst_ip_address,
						&current_packet.dao.dao,
						(current_packet.dao.has_target)? &current_packet.dao.target : NULL,
						(current_packet.dao.has_transit)? &current_packet.dao.transit : NULL);
				break;

			case RPT_Data:
				rpl_collector_parse_data(
						current_packet.src_wpan_address,
						current_packet.dst_wpan_address,
						&current_packet.src_ip_address,
						&current_packet.dst_ip_address,
						(current_packet.data.has_hop_info)? &current_packet.data.hop_info : NULL,
						current_packet.packet_id);
				break;

			case RPT_RplUnknown:
				fprintf(stderr, "Warning: invalid RPL packet\n");
				break;

			case RPT_None:
				break;
		}
	}
}
