/*
 * File:   address.h
 * Author: am
 *
 * Created on June 20, 2013, 4:33 PM
 */

#ifndef ADDRESS_H
#define	ADDRESS_H

#include <arpa/inet.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define ADDR_MAC64_BROADCAST 0xFFFF

typedef struct in6_addr addr_ipv6_t;
typedef uint64_t addr_wpan_t;

typedef struct di_prefix {
	uint8_t length;
	addr_ipv6_t prefix;
	time_t expiration_time;
} di_prefix_t;

typedef struct wpan_addr_list {
	addr_wpan_t address;
	struct wpan_addr_list* next;
} wpan_addr_elt_t, *wpan_addr_list_t;

wpan_addr_elt_t *addr_wpan_add_to_list(wpan_addr_list_t *list, addr_wpan_t address);
wpan_addr_elt_t *addr_wpan_del_from_list(wpan_addr_list_t *list, addr_wpan_t address);

uint64_t addr_get_mac64_from_ip(addr_ipv6_t ip_address);
addr_ipv6_t addr_get_local_ip_from_mac64(addr_wpan_t mac_address);
addr_ipv6_t addr_get_global_ip_from_mac64(di_prefix_t prefix, addr_wpan_t mac_address);
uint64_t addr_get_int_id_from_mac64(addr_wpan_t mac_address);
addr_wpan_t addr_get_mac64_from_int_id(uint64_t int_id);

//For list search
int addr_compare_ip(const addr_ipv6_t *a, const addr_ipv6_t *b);
int addr_compare_wpan(const addr_wpan_t *a, const addr_wpan_t *b);
int addr_compare_ip_len(const addr_ipv6_t *a, const addr_ipv6_t *b, int bit_len);	//Compare len bits

bool addr_is_ip_any(addr_ipv6_t address);
bool addr_is_ip_multicast(addr_ipv6_t address);
bool addr_is_ip_local(addr_ipv6_t address);
bool addr_is_ip_global(addr_ipv6_t address);
bool addr_is_mac64_broadcast(addr_wpan_t address);

#ifdef	__cplusplus
}
#endif

#endif	/* ADDRESS_H */

