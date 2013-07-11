/*
 * File:   versioned_data.h
 * Author: am
 *
 * Created on July 4, 2013, 10:29 AM
 */

#ifndef VERSIONED_DATA_H
#define	VERSIONED_DATA_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "../uthash.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_versioned_data_el di_versioned_data_el_t;

typedef uint64_t di_version_t;

typedef struct di_version_ref {
	di_version_t number;
	di_versioned_data_el_t *element;
} di_version_ref_t;

typedef struct di_versioned_data_el {
	di_version_t version;
	void *data;
	UT_hash_handle hh;
} di_versioned_data_el_t, *di_versioned_data_hash_t;

typedef struct di_versioned_data {
	di_versioned_data_hash_t data_list;
	void *working_address;
	size_t data_size;
	di_version_t latest_version;
	di_versioned_data_el_t *last_checkout;
	pthread_mutex_t mutex;
	pthread_mutex_t data_mutex;
} *di_versioned_data_t;

//An array pointer for the same object but in different working location
typedef struct di_versioned_pointer {
	void **working_addresses;
	size_t working_loc_count;
} di_versioned_pointer_t;

di_versioned_data_t verdata_create(void* working_address, size_t data_size);
void verdata_lock_data(di_versioned_data_t handle);
void verdata_unlock_data(di_versioned_data_t handle);
di_version_t verdata_commit(di_versioned_data_t handle);
void verdata_commit_version(di_versioned_data_t handle, di_version_t version);
bool verdata_checkout(di_versioned_data_t handle, di_version_t version);
di_version_t verdata_checkout_next(di_versioned_data_t handle);
di_version_t verdata_checkout_prev(di_versioned_data_t handle);
bool verdata_checkout_ref(di_versioned_data_t handle, di_version_ref_t version_ref);
di_version_ref_t verdata_nearest_version(di_versioned_data_t handle, di_version_t version);
void verdata_destroy(di_versioned_data_t handle);

#define verdata_dref_ptr(versioned_ptr, type, working_location_id) ( ((type)*) ((versioned_ptr)->working_addresses[(working_location_id)]))
#define verdata_set_ptr(versioned_ptr, target, working_location_id) ((versioned_ptr)->working_addresses[(working_location_id)] = target)
di_versioned_pointer_t *verdata_create_ptr(size_t working_loc_count);
void verdata_destroy_ptr(di_versioned_pointer_t ptr);

#ifdef	__cplusplus
}
#endif

#endif	/* VERSIONED_DATA_H */
