#include "property.h"

property_pool_t property_create_pool() {
	property_pool_t pool;
	
	pool = (property_pool_t) calloc(1, sizeof(property_pool_t));
	return pool;
}

property_data_t *property_set(property_pool_t pool, const char* name, property_data_t data) {
	property_t *property;
	
	HASH_FIND_STR(pool->head, name, property);
	if(!property) {
		property = (property_t*) calloc(1, sizeof(property_t));
		property->name = strdup(name);
		property->data = data;
		HASH_ADD_STR(pool->head, name, property);
	} else {
		property->data = data;
	}
	
	return &property->data;
}

property_data_t *property_get(property_pool_t pool, const char* name) {
	property_t *property;
	
	HASH_FIND_STR(pool->head, name, property);

	if(property)
		return &property->data;
	else return NULL;
}

property_data_t property_remove(property_pool_t pool, const char* name) {
	property_t *property;
	property_data_t property_data;
	
	HASH_FIND_STR(pool->head, name, property);
	if(!property) {
		property_data.type = PT_None;
	} else {
		property_data = property->data;
		HASH_DEL(pool->head, property);
	}
	
	return property_data;
}

void property_free(property_pool_t pool, delete_iterator delete_function) {
	property_t *property, *tmp;

	HASH_ITER(hh, pool->head, property, tmp) {
		if(delete_function)
			delete_function(property->data.valuePtr);

		HASH_DEL(pool->head, property);
		free((void*)property->name);
		property->name = NULL;
		property->data.type = PT_None;
		free(property);
	}
	
	free(pool);
}

