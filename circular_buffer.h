/* 
 * File:   circular_buffer.h
 * Author: am
 *
 * Created on June 14, 2013, 9:50 AM
 */

#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct circular_buffer {
	uint8_t *data;
	int size;
	int element_size;
	int read_idx;
	int write_idx;
} *circular_buffer_t;

circular_buffer_t circular_buffer_create(int size, int element_size);
void circular_buffer_delete(circular_buffer_t buf);
bool circular_buffer_is_empty(circular_buffer_t buf);
bool circular_buffer_is_full(circular_buffer_t buf);
bool circular_buffer_push_front(circular_buffer_t buf, const void *data);
void *circular_buffer_pop_back(circular_buffer_t buf);


#endif	/* CIRCULAR_BUFFER_H */

