#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../utlist.h"
#include "interfaces_register.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>

typedef struct interface_el {
	interface_t interface;
	struct interface_el *next;
} interface_el_t, *interface_list_t;

interface_list_t interfaces;

static interface_el_t *interface_register_from_shared_obj(const char* filename);
static interface_el_t *interface_add(interface_t *interface);

void interface_register_all() {
	static const char *dir = ".";
	char filename_qfd[100];

	interfaces = NULL;

	struct dirent *dp;
	DIR *dfd;
	dfd = opendir(dir);
	if(!dfd)
		goto error;
	
	while((dp = readdir(dfd)) != NULL) {
		struct stat stbuf;
		sprintf(filename_qfd , "%s/%s", dir, dp->d_name);
		if(stat(filename_qfd, &stbuf) == -1)
			continue;
		
		if((stbuf.st_mode & S_IFMT) == S_IFREG) {
			interface_register_from_shared_obj(filename_qfd);
		}
	}
	
	closedir(dfd);
	
error:
	;
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
	
	//If we couldn't find the interface in registered interfaces, try to find it if the interface name is a interface shared object file

	interface_element = interface_register_from_shared_obj(name);
	if(interface_element == NULL)
		return false;
	
	interface_element->interface.init(target);
	return true;
}

static interface_el_t *interface_register_from_shared_obj(const char* filename) {
	void* handle;
	const char* error;
	interface_register_function_t interface_register;
	interface_t interface_description;
	interface_el_t *interface_element;

	handle = dlopen(filename, RTLD_NOW);
	if(handle == NULL)
		return NULL;

	dlerror(); //clear errors
	*(void**)(&interface_register) = dlsym(handle, "interface_register");
	if((error = dlerror()) || interface_register == NULL) {
		dlclose(handle);
		fprintf(stderr, "Can't register interface file %s: %s\n", filename, error);
		return NULL;
	}
	interface_description = interface_register();
	interface_element = interface_add(&interface_description);
	
	if(interface_element == NULL) {
		fprintf(stderr, "Can't register interface file %s: %s\n", filename, "Could not add the interface to interface list");
		return NULL;
	}

	fprintf(stderr, "Registered interface %s from file %s\n", interface_description.interface_name, filename);
	
	
	return interface_element;
}

static interface_el_t *interface_add(interface_t *interface) {
	interface_el_t *interface_element;
	
	interface_element = (interface_el_t*) malloc(sizeof(interface_el_t));
	interface_element->interface = *interface;
	LL_PREPEND(interfaces, interface_element);
	
	return interface_element;
}
