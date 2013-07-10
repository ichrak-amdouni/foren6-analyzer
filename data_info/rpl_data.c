#include "rpl_data.h"

di_rpl_data_t collected_data;

void rpldata_init() {
	collected_data.nodes = hash_create(node_sizeof(), (void(*)(void*))&node_init);
	collected_data.dodags = hash_create(sizeof(di_dodag_t), (void(*)(void*))&dodag_init);
	collected_data.rpl_instances = hash_create(sizeof(di_rpl_instance_t), (void(*)(void*))&rpl_instance_init);
	collected_data.links = hash_create(sizeof(di_link_t), (void(*)(void*))&link_init);
}

di_rpl_data_t *rpldata_get() {
	return &collected_data;
}