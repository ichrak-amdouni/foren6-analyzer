#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdint.h>

#include "../utlist.h"
#include "interfaces_mgr.h"
#include "../sha1.h"
#include "../sniffer_packet_parser.h"

#define LAST_PACKET_NUMBER 100
#define MINIMAL_CAPTURE_VERSION 0
#define RECOMMENDED_CAPTURE_VERSION 1

static pthread_mutex_t packet_reception_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef ifinstance_t* ifinstance_list_t;

struct packet_data {
	struct timeval timestamp;
	unsigned char *data;
	int len;
};

typedef struct interface_el {
	interface_t interface;
	struct interface_el *next;
} interface_el_t, *interface_list_t;

interface_list_t interfaces;
ifinstance_list_t interface_handles;

static interface_el_t *interfacemgr_register_from_shared_obj(const char* filename);
static interface_el_t *interfacemgr_add(interface_t *interface);

void interfacemgr_enumerate(interface_enum_function_t callback) {
	interface_el_t *interface_element;
	
	LL_FOREACH(interfaces, interface_element)
		callback(interface_element->interface.interface_name);
}

interface_t *interfacemgr_get(const char *name) {
	interface_el_t *interface_element;
	
	LL_FOREACH(interfaces, interface_element) {
		if(!strcmp(interface_element->interface.interface_name, name)) {
			return &interface_element->interface;
		}
	}
	
	//If we couldn't find the interface in registered interfaces, try to find it if the interface name is a interface shared object file

	interface_element = interfacemgr_register_from_shared_obj(name);
	if(interface_element == NULL)
		return NULL;
	
	return &interface_element->interface;
}

static interface_el_t *interfacemgr_register_from_shared_obj(const char* filename) {
	void* handle;
	const char* error;
	interface_version_function_t interface_get_version;
	interface_register_function_t interface_register;
	interface_t interface_description;
	interface_el_t *interface_element;

	int interface_version;

	handle = dlopen(filename, RTLD_NOW);
	if(handle == NULL)
		return NULL;

	dlerror(); //clear errors
	*(void**)(&interface_get_version) = dlsym(handle, "interface_get_version");
	if((error = dlerror()) || interface_get_version == NULL) {
		interface_version = 0;
	} else {
		interface_version = interface_get_version();
	}

	if(interface_version < MINIMAL_CAPTURE_VERSION) {
		fprintf(stderr, "Can't register interface file %s: too old version: %d\n", filename, interface_version);
		return NULL;
	} else if(interface_version < RECOMMENDED_CAPTURE_VERSION) {
		fprintf(stderr, "Warning: interface file %s has a deprecated version: %d\n", filename, interface_version);
		return NULL;
	}

	dlerror(); //clear errors
	*(void**)(&interface_register) = dlsym(handle, "interface_register");
	if((error = dlerror()) || interface_register == NULL) {
		dlclose(handle);
		fprintf(stderr, "Can't register interface file %s: %s\n", filename, error);
		return NULL;
	}
	interface_description = interface_register();
	interface_element = interfacemgr_add(&interface_description);
	
	if(interface_element == NULL) {
		fprintf(stderr, "Can't register interface file %s: %s\n", filename, "Could not add the interface to interface list");
		return NULL;
	}

	fprintf(stderr, "Registered interface %s from file %s\n", interface_description.interface_name, filename);
	
	
	return interface_element;
}

static interface_el_t *interfacemgr_add(interface_t *interface) {
	interface_el_t *interface_element;
	
	interface_element = (interface_el_t*) malloc(sizeof(interface_el_t));
	interface_element->interface = *interface;

	LL_PREPEND(interfaces, interface_element);
	
	return interface_element;
}

ifinstance_t* interfacemgr_create_handle() {
	ifreader_t handle = calloc(1, sizeof(struct ifinstance));


	handle->last_packets = hash_create(sizeof(struct packet_data), NULL);
	handle->first_packet = true;

	LL_PREPEND(interface_handles, handle);

	return handle;
}

void interfacemgr_destroy_handle(ifinstance_t* handle) {
	LL_DELETE(interface_handles, handle);
	hash_destroy(handle->last_packets);
	free(handle);
}

struct timeval interfacemgr_get_absolute_timestamp(const ifinstance_t* iface, struct timeval packet_timestamp) {
	struct timeval res = packet_timestamp;

	const ifinstance_t* p;

	for(p = iface; p != 0; p = p->parent) {
		timeradd(&res, &p->delta_to_parent, &res);
	}

	return res;
}

void interfacemgr_set_origin(ifinstance_t* iface, struct timeval packet_timestamp) {
	struct timeval parent_offset;
	struct timeval zero = {0, 0};

	parent_offset = interfacemgr_get_absolute_timestamp(iface->parent, packet_timestamp);

	timersub(&zero, &parent_offset, &iface->delta_to_parent);
}

void inferfacemgr_set_master_time(ifinstance_t* iface, struct timeval packet_timestamp) {
	iface->parent = 0;
	interfacemgr_set_origin(iface, packet_timestamp);
}

ifinstance_t *interfacemgr_get_root(ifinstance_t* iface) {
	ifinstance_t* p;

	for(p = iface; p->parent != 0; p = p->parent);

	interfacemgr_rebase_parent(iface);

	return p;
}

void interfacemgr_rebase_parent(ifinstance_t* iface) {
	struct timeval res;
	ifinstance_t* p;

	timerclear(&res);

	for(p = iface; p->parent != 0; p = p->parent) {
		timeradd(&res, &p->delta_to_parent, &res);
	}

	if(p != iface) {
		iface->parent = p;
		iface->delta_to_parent = res;
	}
}

void interfacemgr_sync_time(ifinstance_t* iface1, struct timeval packet_timestamp1, ifinstance_t* iface2, struct timeval packet_timestamp2) {
	ifinstance_t *root1, *root2;
	struct timeval zero = {0, 0};

	root1 = interfacemgr_get_root(iface1);
	root2 = interfacemgr_get_root(iface2);

	if(root1 == root2)
		return;


	if(!iface1->parent && !iface2->parent) {
		iface1->parent = iface2;
		timersub(&packet_timestamp2, &packet_timestamp1, &iface1->delta_to_parent);
	} else if(!iface1->parent && iface2->parent) {
		iface1->parent = iface2;
		timersub(&packet_timestamp2, &packet_timestamp1, &iface1->delta_to_parent);
	} else if(iface1->parent && !iface2->parent) {
		iface2->parent = iface1;
		timersub(&packet_timestamp1, &packet_timestamp2, &iface2->delta_to_parent);
	} else {
		ifinstance_t *old_parent, *p, *child;
		struct timeval parent_delta, temp;

		timersub(&packet_timestamp1, &packet_timestamp2, &parent_delta);

		//Merge synchronisation trees by switch parents of a tree
		for(child = iface1, p = iface2, old_parent = iface2->parent; p != 0;) {
			p->parent = child;

			temp = p->delta_to_parent;
			p->delta_to_parent = parent_delta;
			timersub(&zero, &temp, &parent_delta); //parent_delta = -temp

			child = p;
			p = old_parent;
			old_parent = p->parent;
		}
	}
}

void interfacemgr_process_packet(ifinstance_t* iface, const unsigned char* data, int len, struct timeval timestamp) {
	pthread_mutex_lock(&packet_reception_mutex);

	if(interfacemgr_check_duplicate_packet(iface, data, len, timestamp)) {
		pthread_mutex_unlock(&packet_reception_mutex);
		return;
	}

	//Set the first packet timestamp to 0
	if(iface->first_packet && iface->parent == NULL) {
		struct timeval zero = {0, 0};
		timersub(&zero, &timestamp, &iface->delta_to_parent);
		iface->first_packet = false;
	}

	ifinstance_t *interface_instance;
	bool packet_was_already_received = false;

	LL_FOREACH(interface_handles, interface_instance) {
		if(interface_instance == iface)
			continue;

		if(interfacemgr_check_multisniffer_packet(iface, interface_instance, data, len, timestamp))
			packet_was_already_received = true;
	}

	if(!packet_was_already_received)
		sniffer_parser_parse_data(data, len , interfacemgr_get_absolute_timestamp(iface, timestamp));

	pthread_mutex_unlock(&packet_reception_mutex);
}

bool interfacemgr_check_multisniffer_packet(ifinstance_t* iface, ifinstance_t* other, const unsigned char* data, int len, struct timeval timestamp) {
	struct packet_data *old_pkt = interfacemgr_get_old_packet(other, data, len);

	if(old_pkt) {
		interfacemgr_sync_time(iface, timestamp, other, old_pkt->timestamp);
		return true;
	}

	return false;
}

bool interfacemgr_check_duplicate_packet(ifinstance_t* iface, const unsigned char* data, int len, struct timeval timestamp) {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	struct packet_data pkt_data;
	bool is_duplicate;
	uint32_t hashed_data[5];
	struct packet_data *old_pkt = NULL;

	sha1_buffer((const char*)data, len, hashed_data);

	if(hash_find(iface->last_packets, (hash_key_t){hashed_data, sizeof(hashed_data)}, it)) {
		old_pkt = (struct packet_data *)hash_it_value(it);
		free(old_pkt->data);
		hash_it_delete_value(it);
		is_duplicate = true;
	} else {
		is_duplicate = false;
		if(hash_size(iface->last_packets) >= LAST_PACKET_NUMBER) {
			//values are added to the end of the hash, so to remove the oldest value, remove the one at the beginning
			struct packet_data *old_pkt;
			hash_begin(iface->last_packets, it);
			old_pkt = (struct packet_data *)hash_it_value(it);
			free(old_pkt->data);
			hash_it_delete_value(it);
		}
	}

	hash_it_destroy(it);

	pkt_data.data = malloc(len);
	memcpy(pkt_data.data, data, len);
	pkt_data.len = len;
	pkt_data.timestamp = timestamp;
	hash_add(iface->last_packets, (hash_key_t){hashed_data, sizeof(hashed_data)}, &pkt_data, NULL, HAM_FailIfExists, NULL);

	return is_duplicate;
}

struct packet_data *interfacemgr_get_old_packet(ifinstance_t* iface, const unsigned char *data, int len) {
	hash_iterator_ptr it = hash_begin(NULL, NULL);
	struct packet_data *old_pkt = NULL;
	uint32_t hashed_data[5];

	sha1_buffer((const char*)data, len, hashed_data);

	if(hash_find(iface->last_packets, (hash_key_t){hashed_data, sizeof(hashed_data)}, it)) {
		old_pkt = (struct packet_data *)hash_it_value(it);
	}

	hash_it_destroy(it);

	return old_pkt;
}

