/*
 * File:   hash_container.h
 * Author: am
 *
 * Created on July 5, 2013, 9:52 AM
 */

#ifndef HASH_CONTAINER_H
#define	HASH_CONTAINER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct hash_key {
	void *key;
	size_t size;
} hash_key_t;

typedef struct hash_container *hash_container_ptr;
typedef struct hash_iterator *hash_iterator_ptr;



typedef enum hash_add_mode {
	HAM_NoCheck,
	HAM_FailIfExists,
	HAM_OverwriteIfExists
} hash_add_mode_e;

typedef enum hash_value_mode {
	HVM_FailIfNonExistant,
	HVM_CreateIfNonExistant
} hash_value_mode_e;

//hash_key_t hash_key_make(T key_value);
#define hash_key_make(key_value) ((hash_key_t){(void*)&(key_value), sizeof(key_value)})

hash_container_ptr hash_create(size_t data_size, void (*data_constructor)(void *data, const void *key, size_t key_size));
hash_container_ptr hash_dup(hash_container_ptr container);
void               hash_destroy(hash_container_ptr container);
bool               hash_add(hash_container_ptr container, hash_key_t key, const void *data, hash_iterator_ptr iterator, hash_add_mode_e mode, bool *was_existing);
bool               hash_add_ref(hash_container_ptr container, hash_key_t key, const void *data, hash_iterator_ptr iterator, hash_add_mode_e mode, bool *was_existing);
void *             hash_value(hash_container_ptr container, hash_key_t key, hash_value_mode_e mode, bool *was_created);
bool               hash_find(hash_container_ptr container, hash_key_t key, hash_iterator_ptr iterator);
void *             hash_remove_ref(hash_container_ptr container, hash_key_t key);
bool               hash_delete(hash_container_ptr container, hash_key_t key);
void               hash_clear(hash_container_ptr container);

hash_iterator_ptr  hash_begin(hash_container_ptr container, hash_iterator_ptr iterator);
hash_iterator_ptr  hash_end(hash_container_ptr container, hash_iterator_ptr iterator);
void               hash_it_destroy(hash_iterator_ptr iterator);

hash_iterator_ptr  hash_it_inc(hash_iterator_ptr iterator);
hash_iterator_ptr  hash_it_dec(hash_iterator_ptr iterator);
bool               hash_it_equ(hash_iterator_ptr iterator, hash_iterator_ptr other_it);
hash_iterator_ptr  hash_it_cpy(hash_iterator_ptr iterator, hash_iterator_ptr new_iterator);
hash_iterator_ptr  hash_it_remove_ref(hash_iterator_ptr iterator);
hash_iterator_ptr  hash_it_delete_value(hash_iterator_ptr iterator);

void *             hash_it_value(hash_iterator_ptr iterator);
void *             hash_it_key(hash_iterator_ptr iterator);


#ifdef	__cplusplus
}
#endif

#endif	/* HASH_CONTAINER_H */

