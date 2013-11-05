#include "6lowpan_def.h"
#include "stddef.h"

void init_sixlowpan_config(sixlowpan_config_t *config) {
    init_ipv6_addr(&config->local_address);
    init_ipv6_addr(&config->global_address);
    config->has_seen_local_address = false;
    config->is_custom_local_address = false;
    config->has_seen_global_address = false;
    config->is_custom_global_address = false;
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

void init_sixlowpan_errors(sixlowpan_errors_t *errors)
{
    errors->invalid_ip = 0;
    errors->invalid_prefix = 0;
}

void init_sixlowpan_errors_delta(sixlowpan_errors_delta_t *delta)
{
    delta->has_changed = false;
    delta->invalid_ip = 0;
    delta->invalid_prefix = 0;
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

void sixlowpan_errors_delta(const sixlowpan_errors_t *left, const sixlowpan_errors_t *right, sixlowpan_errors_delta_t *delta) {
    if ( delta == NULL ) return;
      if ( left == NULL && right == NULL ) {
        delta->invalid_ip = 0;
        delta->invalid_prefix = 0;
    } else if ( left == NULL || right == NULL ) {
        delta->invalid_ip = left ? left->invalid_ip : right->invalid_ip;
        delta->invalid_prefix = left ? left->invalid_prefix : right->invalid_prefix;
    } else {
        delta->invalid_ip = right->invalid_ip - left->invalid_ip;
        delta->invalid_prefix = right->invalid_prefix - left->invalid_prefix;
    }
    delta->has_changed = delta->invalid_ip != 0 || delta->invalid_prefix;
}
