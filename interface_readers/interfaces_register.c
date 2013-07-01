#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../utlist.h"

#include "interface_telos.h"

typedef struct interface_el {
	interface_t interface;
	struct interface_el *next;
} interface_el_t, *interface_list_t;

interface_list_t interfaces;

static void interface_add(interface_t interface);

void interface_register_all() {
	interfaces = NULL;
	
	interface_add(interface_register_telos());
}

void interface_enumerate(interface_enum_function_t callback) {
	interface_el_t *interface_element;
	
	LL_FOREACH(interfaces, interface_element)
		callback(interface_element->interface.interface_name);
}

bool interface_init(const char *name, const char *target) {
	interface_el_t *interface_element;
	
	LL_FOREACH(interfaces, interface_element) {
		if(!strcmp(interface_element->interface.interface_name, name)) {
			interface_element->interface.init(target);
			return true;
		}
	}
	
	return false;
}

static void interface_add(interface_t interface) {
	interface_el_t *interface_element;
	
	interface_element = (interface_el_t*) malloc(sizeof(interface_el_t));
	interface_element->interface = interface;
	LL_PREPEND(interfaces, interface_element);
}
