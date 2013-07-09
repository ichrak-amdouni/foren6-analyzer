#include <string.h>
#include <stdlib.h>

#include "node.h"
#include "route.h"

void node_init(di_node_t *node) {
	node = node;
	node->dodag.version = -1;
}

di_node_t *node_dup(di_node_t *node) {
	di_node_t *new_node;

	new_node = malloc(sizeof(di_node_t));
	memcpy(new_node, node, sizeof(di_node_t));
	new_node->routes = route_dup(&node->routes);

	return new_node;
}
