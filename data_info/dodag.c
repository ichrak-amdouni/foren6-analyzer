#include <stdlib.h>
#include <string.h>

#include "dodag.h"
#include "node.h"

void dodag_init(di_dodag_t *dodag) {
	dodag->nodes = hash_create(sizeof(di_node_key_t), NULL);
	dodag->rpl_instance.rpl_instance = -1;
}

di_dodag_t *dodag_dup(di_dodag_t *dodag) {
	di_dodag_t *new_dodag;

	new_dodag = malloc(sizeof(di_dodag_t));
	memcpy(new_dodag, dodag, sizeof(di_dodag_t));
	new_dodag->nodes = hash_dup(dodag->nodes);

	return new_dodag;
}
