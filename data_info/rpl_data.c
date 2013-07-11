#include "rpl_data.h"

di_rpl_data_t collected_data;

void rpldata_init() {
	collected_data.nodes = hash_create(node_sizeof(), &node_init);
	collected_data.dodags = hash_create(dodag_sizeof(), &dodag_init);
	collected_data.rpl_instances = hash_create(rpl_instance_sizeof(), &rpl_instance_init);
	collected_data.links = hash_create(link_sizeof(), &link_init);
}

di_rpl_data_t *rpldata_get() {
	return &collected_data;
}

di_node_t *rpldata_get_node(const di_node_key_t *node_key) {
	return hash_value(collected_data.nodes, hash_key_make(*node_key), HVM_FailIfNonExistant, NULL);
}

di_dodag_t *rpldata_get_dodag(const di_dodag_key_t *dodag_key) {
	return hash_value(collected_data.dodags, hash_key_make(*dodag_key), HVM_FailIfNonExistant, NULL);
}

di_rpl_instance_t *rpldata_get_rpl_instance(const di_rpl_instance_key_t *rpl_instance_key) {
	return hash_value(collected_data.rpl_instances, hash_key_make(*rpl_instance_key), HVM_FailIfNonExistant, NULL);
}

di_link_t *rpldata_get_link(const di_link_key_t *link_key) {
	return hash_value(collected_data.links, hash_key_make(*link_key), HVM_FailIfNonExistant, NULL);
}
