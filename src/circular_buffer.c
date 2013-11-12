/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         Circular buffers for sniffer inputs
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include <stdlib.h>
#include <string.h>

#include "circular_buffer.h"


static void circular_buffer_inc_write_idx(circular_buffer_t buf);
static void circular_buffer_inc_read_idx(circular_buffer_t buf);

circular_buffer_t circular_buffer_create(int size, int element_size) {
	circular_buffer_t new_buffer = (circular_buffer_t) malloc(sizeof(struct circular_buffer));
	
	if(!new_buffer)
		goto error;
	
	new_buffer->data = (uint8_t*) malloc(size*element_size);
	if(!new_buffer->data)
		goto error;
	
	new_buffer->size = size;
	new_buffer->element_size = element_size;
	new_buffer->read_idx = new_buffer->write_idx = 0;
	
	return new_buffer;
	
error:
	if(new_buffer) {
		if(new_buffer->data)
			free(new_buffer->data);
		free(new_buffer);
	}

	return NULL;
}

void circular_buffer_delete(circular_buffer_t buf) {
	free(buf->data);
	free(buf);
}

bool circular_buffer_is_empty(circular_buffer_t buf) {
	return buf->read_idx == buf->write_idx;
}

bool circular_buffer_is_full(circular_buffer_t buf) { 
	int next_index = buf->write_idx + 1;
	if(next_index >= buf->size)
		next_index = 0;
	
	return buf->read_idx == next_index;
}


bool circular_buffer_push_front(circular_buffer_t buf, const void *data) {
	if(circular_buffer_is_full(buf))
		return false;
	
	memcpy(&buf->data[buf->write_idx * buf->element_size], data, buf->element_size);
	
	circular_buffer_inc_write_idx(buf);
	
	return true;
}

void *circular_buffer_pop_back(circular_buffer_t buf) {
	void *data_ptr;
	
	if(circular_buffer_is_empty(buf))
		return NULL;
	
	data_ptr = &(buf->data[buf->read_idx * buf->element_size]);
	
	circular_buffer_inc_read_idx(buf);
	
	return data_ptr;
}

static void circular_buffer_inc_write_idx(circular_buffer_t buf) {
	int new_index = buf->write_idx + 1;
	if(new_index >= buf->size)
		buf->write_idx = 0;
	else buf->write_idx = new_index;
}


static void circular_buffer_inc_read_idx(circular_buffer_t buf) {
	int new_index = buf->read_idx + 1;
	if(new_index >= buf->size)
		buf->read_idx = 0;
	else buf->read_idx = new_index;
}