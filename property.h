/* 
 * File:   property.h
 * Author: am
 *
 * Created on June 18, 2013, 11:03 AM
 */

#ifndef PROPERTY_H
#define	PROPERTY_H
 
#include "uthash.h"

struct property;

typedef struct property_pool {
	struct property *head;
} *property_pool_t;

typedef enum property_type {
	PT_None,
	PT_Integer,
	PT_Pointer,
	PT_String,
	PT_Real
} property_type_e;

typedef struct property_data {
	property_type_e type;
	union {
		long int valueInt;
		void* valuePtr;
		const char* valueStr;
		double valueReal;
	};
} property_data_t;

typedef struct property {
	const char* name;
	property_data_t data;
    UT_hash_handle hh;
} property_t;

typedef void (*delete_iterator)(void* data_ptr);

//Create a property pool
property_pool_t property_create_pool();

//Set or add a name/value pair
property_data_t *property_set(property_pool_t pool, const char* name, property_data_t data);

//Get a property value (NULL if not found)
property_data_t *property_get(property_pool_t pool, const char* name);

//Remove a property and retrieve it's data
property_data_t property_remove(property_pool_t pool, const char* name);

//Free memory used by the property pool. if delete_function is not NULL, it's called for each property data (can be &free if the data is a pointer to allocated memory)
void property_free(property_pool_t pool, delete_iterator delete_function);


#endif	/* PROPERTY_H */

