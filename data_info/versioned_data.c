#include <assert.h>

#include "versioned_data.h"
#include "../uthash.h"


di_versioned_data_t verdata_create(void* working_address, size_t data_size) {
	di_versioned_data_t new_versioned_data;
	
	new_versioned_data = calloc(1, sizeof(struct di_versioned_data));
	new_versioned_data->working_address = working_address;
	new_versioned_data->data_size = data_size;
	pthread_mutex_init(&new_versioned_data->mutex, NULL);
	
	return new_versioned_data;
}

void verdata_lock_data(di_versioned_data_t handle) {
	pthread_mutex_lock(&handle->data_mutex);
}

void verdata_unlock_data(di_versioned_data_t handle) {
	pthread_mutex_unlock(&handle->data_mutex);
}

di_version_t verdata_commit(di_versioned_data_t handle) {
	di_versioned_data_el_t *new_version;
	
	new_version = calloc(1, sizeof(di_versioned_data_el_t));
	new_version->data = malloc(handle->data_size);
	
	verdata_lock_data(handle);
		memcpy(new_version->data, handle->working_address, handle->data_size);
	verdata_unlock_data(handle);

	pthread_mutex_lock(&handle->mutex);
		new_version->version = ++handle->latest_version;

		HASH_ADD(hh, handle->data_list, version, sizeof(di_version_t), new_version);
	pthread_mutex_unlock(&handle->mutex);
	
	return new_version->version;
}

void verdata_commit_version(di_versioned_data_t handle, di_version_t version) {
	di_versioned_data_el_t *new_version;
	
	new_version = calloc(1, sizeof(di_versioned_data_el_t));
	new_version->data = malloc(handle->data_size);
	
	verdata_lock_data(handle);
		memcpy(new_version->data, handle->working_address, handle->data_size);
	verdata_unlock_data(handle);
	
	new_version->version = version;

	pthread_mutex_lock(&handle->mutex);
		if(handle->latest_version < version)
			handle->latest_version = version;

		HASH_ADD(hh, handle->data_list, version, sizeof(di_version_t), new_version);
	pthread_mutex_unlock(&handle->mutex);
}

bool verdata_checkout(di_versioned_data_t handle, di_version_t version) {
	di_versioned_data_el_t *data;
	
	pthread_mutex_lock(&handle->mutex);
		HASH_FIND(hh, handle->data_list, &version, sizeof(di_version_t), data);
	pthread_mutex_unlock(&handle->mutex);

	if(data) {
		verdata_lock_data(handle);
			memcpy(handle->working_address, data->data, handle->data_size);
		verdata_unlock_data(handle);
		return true;
	}
	
	return false;
}

di_version_t verdata_checkout_next(di_versioned_data_t handle) {
	di_versioned_data_el_t *next_data = (di_versioned_data_el_t*) handle->last_checkout->hh.next;

	if(!next_data) {
		if(handle->last_checkout)
			return handle->last_checkout->version;
		else return handle->latest_version;
	}
	
	verdata_lock_data(handle);
		memcpy(handle->working_address, next_data->data, handle->data_size);
	verdata_unlock_data(handle);
	
	pthread_mutex_lock(&handle->mutex);
		handle->last_checkout = next_data;
	pthread_mutex_unlock(&handle->mutex);
	
	return next_data->version;
}

di_version_t verdata_checkout_prev(di_versioned_data_t handle) {
	di_versioned_data_el_t *next_data = (di_versioned_data_el_t*) handle->last_checkout->hh.prev;

	if(!next_data) {
		if(handle->last_checkout)
			return handle->last_checkout->version;
		else return handle->latest_version;
	}
	
	verdata_lock_data(handle);
		memcpy(handle->working_address, next_data->data, handle->data_size);
	verdata_unlock_data(handle);
	
	pthread_mutex_lock(&handle->mutex);
		handle->last_checkout = next_data;
	pthread_mutex_unlock(&handle->mutex);
	
	return next_data->version;
}

bool verdata_checkout_ref(di_versioned_data_t handle, di_version_ref_t version_ref) {
	di_versioned_data_el_t *next_data = version_ref.element;
	
	if(!next_data) {
		return false;
	}

	verdata_lock_data(handle);
		memcpy(handle->working_address, next_data->data, handle->data_size);
	verdata_unlock_data(handle);
	
	pthread_mutex_lock(&handle->mutex);
		handle->last_checkout = next_data;
	pthread_mutex_unlock(&handle->mutex);
	
	return true;
}

di_version_ref_t verdata_nearest_version(di_versioned_data_t handle, di_version_t version) {
	di_version_ref_t best_version;
	di_versioned_data_el_t *data_el, *tmp;
	
	best_version.element = NULL;
	best_version.number = 0;
	
	HASH_ITER(hh, handle->data_list, data_el, tmp) {
		if(data_el->version > best_version.number  && data_el->version < version) {
			best_version.number = data_el->version;
			best_version.element = data_el;
		}
	}
	
	return best_version;
}

void verdata_destroy(di_versioned_data_t handle) {
	di_versioned_data_el_t *data_el, *tmp;
	
	HASH_ITER(hh, handle->data_list, data_el, tmp) {
		HASH_DEL(handle->data_list, data_el);
		free(data_el->data);
		free(data_el);
	}
	
	assert(handle->data_list == NULL);
	
	free(handle);
}
