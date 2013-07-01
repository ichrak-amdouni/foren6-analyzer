/* 
 * File:   ipv6_utils.h
 * Author: am
 *
 * Created on June 20, 2013, 11:30 AM
 */

#ifndef IPV6_UTILS_H
#define	IPV6_UTILS_H

#include <arpa/inet.h>

typedef struct wpan_addr_list {
	uint64_t address;
	struct wpan_addr_list* next;
} wpan_addr_elt_t, *wpan_addr_list_t;

wpan_addr_list_t *addr_utils_wpan_add_to_list(wpan_addr_list_t list, uint64_t address);
wpan_addr_list_t *addr_utils_wpan_del_from_list(wpan_addr_list_t list, uint64_t address);

uint64_t addr_utils_get_mac64_from_ip(struct in6_addr address);
struct in6_addr addr_utils_get_local_ip_from_mac64(uint64_t mac_address);
struct in6_addr addr_utils_get_global_ip_from_mac64(struct in6_addr prefix, uint8_t prefix_len, uint64_t mac_address);
uint64_t addr_utils_get_int_id_from_mac64(uint64_t mac_address);
uint64_t addr_utils_get_mac64_from_int_id(uint64_t int_id);

#endif	/* IPV6_UTILS_H */

