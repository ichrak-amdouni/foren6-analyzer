#include "rpl_def.h"
#include "stddef.h"

void rpl_instance_config_compare(const rpl_instance_config_t *left, const rpl_instance_config_t *right, rpl_instance_config_delta_t *delta) {
    if ( delta == NULL ) return;
    if ( left == NULL && right == NULL ) {
        delta->rpl_instance_id = false;
        delta->version_number = false;
        delta->rank = false;
        delta->grounded = false;
        delta->mode_of_operation = false;
        delta->dtsn = false;
        delta->dodagid = false;
    } else if ( left == NULL || right == NULL ) {
        delta->rpl_instance_id = true;
        delta->version_number = true;
        //delta->rank = true;
        delta->grounded = true;
        delta->mode_of_operation = true;
        delta->dtsn = true;
        delta->dodagid = true;
    } else {
        delta->rpl_instance_id = right->rpl_instance_id != left->rpl_instance_id;
        delta->version_number = right->version_number != left->version_number;
        delta->rank = right->rank != left->rank;
        delta->grounded = right->grounded != left->grounded;
        delta->mode_of_operation = right->mode_of_operation != left->mode_of_operation;
        delta->dtsn = right->dtsn != left->dtsn;
        delta->dodagid = addr_compare_ip(&right->dodagid, &left->dodagid) != 0;
    }
    delta->has_changed = delta->rpl_instance_id || delta->version_number ||/* delta->rank ||*/
        delta->grounded || delta->mode_of_operation || /* delta->dtsn ||*/
        delta->dodagid;
}

void rpl_dodag_config_compare(const rpl_dodag_config_t *left, const rpl_dodag_config_t *right, rpl_dodag_config_delta_t *delta) {
    if ( delta == NULL ) return;
    if ( left == NULL && right == NULL ) {
        delta->auth_enabled = false;
        delta->path_control_size = false;
        delta->dio_interval_min = false;
        delta->dio_interval_max = false;
        delta->dio_redundancy_constant = false;
        delta->max_rank_inc = false;
        delta->min_hop_rank_inc = false;
        delta->default_lifetime = false;
        delta->lifetime_unit = false;
        delta->objective_function = false;
    } else if ( left == NULL || right == NULL ) {
        delta->auth_enabled = true;
        delta->path_control_size = true;
        delta->dio_interval_min = true;
        delta->dio_interval_max = true;
        delta->dio_redundancy_constant = true;
        delta->max_rank_inc = true;
        delta->min_hop_rank_inc = true;
        delta->default_lifetime = true;
        delta->lifetime_unit = true;
        delta->objective_function = true;
    } else {
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

    delta->has_changed = delta->auth_enabled || delta->path_control_size || delta->dio_interval_min ||
        delta->dio_interval_max || delta->dio_redundancy_constant || delta->max_rank_inc ||
        delta->min_hop_rank_inc || delta->default_lifetime || delta->lifetime_unit ||
        delta->objective_function;
}

void rpl_prefix_compare(const rpl_prefix_t *left, const rpl_prefix_t *right, rpl_prefix_delta_t *delta) {
    if ( delta == NULL ) return;
    if ( left == NULL && right == NULL ) {
        delta->prefix = false;
        delta->on_link = false;
        delta->auto_address_config = false;
        delta->router_address = false;
        delta->valid_lifetime = false;
        delta->preferred_lifetime = false;
    } else if ( left == NULL || right == NULL ) {
        delta->prefix = true;
        delta->on_link = true;
        delta->auto_address_config = true;
        delta->router_address = true;
        delta->valid_lifetime = true;
        delta->preferred_lifetime = true;
    } else {
        delta->prefix = prefix_compare(&right->prefix, &left->prefix) != 0;
        delta->on_link = right->on_link != left->on_link;
        delta->auto_address_config = right->auto_address_config != left->auto_address_config;
        delta->router_address = right->router_address != left->router_address;
        delta->valid_lifetime = right->valid_lifetime != left->valid_lifetime;
        delta->preferred_lifetime = right->preferred_lifetime != left->preferred_lifetime;
    }

    delta->has_changed = delta->prefix || delta->on_link || delta->auto_address_config ||
        delta->router_address || delta->valid_lifetime || delta->preferred_lifetime;
}
