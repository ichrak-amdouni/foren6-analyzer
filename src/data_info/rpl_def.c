#include "rpl_def.h"

void rpl_dodag_config_compare(const rpl_dodag_config_t *left, const rpl_dodag_config_t *right, rpl_dodag_config_delta_t *delta) {
    if ( left == NULL || right == NULL || delta == NULL ) return;
    delta->auth_enabled = right->auth_enabled != left->auth_enabled;
    delta->path_control_size = right->path_control_size != left->path_control_size;
    delta->dio_interval_min = right->dio_interval_min != left->dio_interval_min;
    delta->dio_interval_max = right->dio_interval_max != left->dio_interval_max;
    delta->dio_redundancy_constant = right->dio_redundancy_constant != left->dio_redundancy_constant;
    delta->max_rank_inc = right->max_rank_inc != left->max_rank_inc;
    delta->min_hop_rank_inc = right->min_hop_rank_inc != left->min_hop_rank_inc;
    delta->default_lifetime = right->default_lifetime != left->default_lifetime;
    delta->lifetime_unit = right->lifetime_unit != left->lifetime_unit;
    delta->objective_function = right->objective_function != left->objective_function;
}

void rpl_prefix_compare(const rpl_prefix_t *left, const rpl_prefix_t *right, rpl_prefix_delta_t *delta) {
    if ( left == NULL || right == NULL || delta == NULL ) return;
    delta->prefix = prefix_compare(&right->prefix, &left->prefix) != 0;
    delta->on_link = right->on_link != left->on_link;
    delta->auto_address_config = right->auto_address_config != left->auto_address_config;
    delta->router_address = right->router_address != left->router_address;
    delta->valid_lifetime = right->valid_lifetime != left->valid_lifetime;
    delta->preferred_lifetime = right->preferred_lifetime != left->preferred_lifetime;
}
