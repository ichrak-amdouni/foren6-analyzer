#include <string.h>
#include <stdlib.h>

#include "rpl_instance.h"
#include "dodag.h"

void rpl_instance_init(di_rpl_instance_t* instance) {
	instance->dodags = hash_create(sizeof(di_dodag_ref_t), NULL);
}

di_rpl_instance_t* rpl_instance_dup(di_rpl_instance_t* rpl_instance) {
	di_rpl_instance_t *new_instance;
	
	new_instance = malloc(sizeof(di_rpl_instance_t));
	memcpy(new_instance, rpl_instance, sizeof(di_rpl_instance_t));
	new_instance->dodags = hash_dup(rpl_instance->dodags);
	
	return new_instance;
}