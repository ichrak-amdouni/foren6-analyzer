#include "rpl_data.h"

di_rpl_data_t collected_data;

void rpldata_init() {
	collected_data.nodes = hash_create(sizeof(di_node_t));
	collected_data.dodags = hash_create(sizeof(di_dodag_t));
	collected_data.rpl_instances = hash_create(sizeof(di_rpl_instance_t));
	collected_data.links = hash_create(sizeof(di_link_t));
}

di_rpl_data_t *rpldata_get() {
	return &collected_data;
}