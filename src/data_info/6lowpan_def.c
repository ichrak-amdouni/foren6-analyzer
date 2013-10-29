#include "6lowpan_def.h"
#include "stddef.h"

void init_sixlowpan_config(sixlowpan_config_t *config) {
    init_ipv6_addr(&config->local_address);
    init_ipv6_addr(&config->global_address);
}

void init_sixlowpan_statistics(sixlowpan_statistics_t *statistics)
{
    statistics->packet_count = 0;
}

void init_sixlowpan_statistics_delta(sixlowpan_statistics_delta_t *delta)
{
    delta->has_changed = false;
    delta->packet_count = 0;
}

void sixlowpan_config_delta(const sixlowpan_config_t *left, const sixlowpan_config_t *right, sixlowpan_config_delta_t *delta) {
    if ( delta == NULL ) return;
      if ( left == NULL && right == NULL ) {
        delta->local_address = false;
        delta->global_address = false;
    } else if ( left == NULL || right == NULL ) {
        delta->local_address = true;
        delta->global_address = true;
    } else {
        delta->local_address = addr_compare_ip(&right->local_address, &left->local_address) != 0;
        delta->global_address = addr_compare_ip(&right->global_address, &left->global_address) != 0;
    }
    delta->has_changed = delta->local_address || delta->global_address;
}

void sixlowpan_statistics_delta(const sixlowpan_statistics_t *left, const sixlowpan_statistics_t *right, sixlowpan_statistics_delta_t *delta) {
    if ( delta == NULL ) return;
      if ( left == NULL && right == NULL ) {
        delta->packet_count = 0;
    } else if ( left == NULL || right == NULL ) {
        delta->packet_count = left ? left->packet_count : right->packet_count;
    } else {
        delta->packet_count = right->packet_count - left->packet_count;
    }
    delta->has_changed = delta->packet_count != 0;
}
